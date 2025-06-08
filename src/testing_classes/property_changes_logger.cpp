#include "error_trigger.h"
#include "logger.h"
#include "property_changes_logger.h"
#include "strutil.h"

#include "godot_cpp/classes/engine.hpp"
#include "godot_cpp/classes/scene_tree.hpp"


using namespace ErrorTrigger;
using namespace godot;


void PropertyChangesLogger::_bind_methods(){
  ClassDB::bind_method(D_METHOD("_on_property_changed"), &PropertyChangesLogger::_on_property_changed);

  ClassDB::bind_method(D_METHOD("get_target_node_path"), &PropertyChangesLogger::get_target_node_path);
  ClassDB::bind_method(D_METHOD("set_target_node_path", "path"), &PropertyChangesLogger::set_target_node_path);

  ClassDB::bind_method(D_METHOD("get_property_watch_list"), &PropertyChangesLogger::get_property_watch_list);
  ClassDB::bind_method(D_METHOD("set_property_watch_list", "watch_list"), &PropertyChangesLogger::set_property_watch_list);

  ClassDB::bind_method(D_METHOD("get_function_watch_list"), &PropertyChangesLogger::get_function_watch_list);
  ClassDB::bind_method(D_METHOD("set_function_watch_list", "watch_list"), &PropertyChangesLogger::set_function_watch_list);

  ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "target_node_path"), "set_target_node_path", "get_target_node_path");
  ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "property_watch_list"), "set_property_watch_list", "get_property_watch_list");
  ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "function_watch_list"), "set_function_watch_list", "get_function_watch_list");
}


void PropertyChangesLogger::_on_property_changed(){
  for(int i = 0; i < _watch_list.size(); i++){
    Variant _key_watch = _watch_list[i];
    Variant _class_variable = _target_node->get(_key_watch);
    if(_class_variable.get_type() == Variant::NIL){
      GameUtils::Logger::print_warn_static(gd_format_str("[PropertyChangesLogger] '{0}' property does not exists or intentionally set to NIL.", _key_watch));
    }

    Variant _stored_variable = _property_data[_key_watch];
    if(_class_variable == _stored_variable)
      continue;

    GameUtils::Logger::print_log_static(gd_format_str("[PropertyChangesLogger] '{0}' property value changed to: {1}", _key_watch, _class_variable));
    _property_data[_key_watch] = _class_variable;
  }
}

void PropertyChangesLogger::_update_function_changes(){
  for(int i = 0; i < _function_watch_list.size(); i++){
    Variant _function_name = _function_watch_list[i];
    Variant _class_variable = _target_node->call(_function_name);
    if(_class_variable.get_type() == Variant::NIL){
      GameUtils::Logger::print_warn_static(gd_format_str("[PropertyChangesLogger] '{0}' function does not exists of intentionally returns NIL.", _function_name));
    }

    Variant _stored_variable = _function_data[_function_name];
    if(_class_variable == _stored_variable)
      continue;

    GameUtils::Logger::print_log_static(gd_format_str("[PropertyChangesLogger] '{0}' function value changed to: {1}", _function_name, _class_variable));
    _function_data[_function_name] = _class_variable;
  }
}


void PropertyChangesLogger::_ready(){
  Engine* _engine = Engine::get_singleton();
  if(_engine->is_editor_hint())
    return;

  int _quit_code;

{ // enclosure for using gotos
  _target_node = get_node<Node>(_target_node_path);
  if(!_target_node){
    GameUtils::Logger::print_err_static("[PropertyChangesLogger] Cannot get target node.");
    _quit_code = ERR_UNCONFIGURED;
    goto on_error;
  }

  _target_node->connect("property_list_changed", Callable(this, "_on_property_changed"));

  _on_property_changed();
} // enclosure closing

  return;


  on_error:{}
  SceneTree* _tree = get_tree();
  trigger_generic_error_message([_tree, _quit_code](){
    _tree->quit(_quit_code);
  });  
}

void PropertyChangesLogger::_process(double delta){
  Engine* _engine = Engine::get_singleton();
  if(_engine->is_editor_hint())
    return;

  _update_function_changes();
}


NodePath PropertyChangesLogger::get_target_node_path() const{
  return _target_node_path;
}

void PropertyChangesLogger::set_target_node_path(const NodePath& path){
  _target_node_path = path;
}


Array PropertyChangesLogger::get_property_watch_list() const{
  return _watch_list;
}

void PropertyChangesLogger::set_property_watch_list(const Array& watch_list){
  _watch_list = watch_list;
}


Array PropertyChangesLogger::get_function_watch_list() const{
  return _function_watch_list;
}

void PropertyChangesLogger::set_function_watch_list(const Array& watch_list){
  _function_watch_list = watch_list;
}
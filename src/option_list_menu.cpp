#include "common_event.h"
#include "error_trigger.h"
#include "gdutils.h"
#include "logger.h"
#include "option_list_menu.h"
#include "option_value_control.h"
#include "strutil.h"

#include "godot_cpp/classes/engine.hpp"
#include "godot_cpp/classes/scene_tree.hpp"


using namespace ErrorTrigger;
using namespace gdutils;
using namespace godot;


const char* OptionListMenu::s_value_set = "value_set";


void OptionListMenu::_bind_methods(){
  ClassDB::bind_method(D_METHOD("_on_option_changed", "obj", "value"), &OptionListMenu::_on_option_changed);

  ClassDB::bind_method(D_METHOD("get_target_list_node"), &OptionListMenu::get_target_list_node);
  ClassDB::bind_method(D_METHOD("set_target_list_node", "path"), &OptionListMenu::set_target_list_node);
  
  ClassDB::bind_method(D_METHOD("set_value_data", "key", "value"), &OptionListMenu::set_value_data);
  ClassDB::bind_method(D_METHOD("get_value_data", "key"), &OptionListMenu::get_value_data);

  ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "target_list_node"), "set_target_list_node", "get_target_list_node");

  ADD_SIGNAL(MethodInfo(SIGNAL_ON_READY, PropertyInfo(Variant::OBJECT, "node")));
  ADD_SIGNAL(MethodInfo(s_value_set, PropertyInfo(Variant::STRING, "key"), PropertyInfo(Variant::NIL, "value")));
}


void OptionListMenu::_on_option_changed(const String& key, const Variant& value){
  emit_signal(s_value_set, key, value);
}


void OptionListMenu::_update_option_nodes(Node* parent){
  for(int i = 0; i < parent->get_child_count(); i++){
    Node* _child_node = parent->get_child(i);
    _update_option_nodes(_child_node);
    
    if(!_child_node->is_class(OptionValueControl::get_class_static()))
      continue;

    _child_node->connect(OptionValueControl::s_value_set, Callable(this, "_on_option_changed"));

    OptionValueControl* _option_control = (OptionValueControl*)_child_node;
    String _option_key = _option_control->get_option_key();
    _option_lists[_option_key] = _option_control;
  }
}


void OptionListMenu::_ready(){
  Engine* _engine = Engine::get_singleton();
  if(_engine->is_editor_hint())
    return;

  int _quit_code;

{ // enclosure for using gotos
  _target_list_node = this;
  if(!_target_list_node_path.is_empty()){
    _target_list_node = get_node<Node>(_target_list_node_path);
    if(!_target_list_node){
      GameUtils::Logger::print_err_static("[OptionListMenu] Cannot get a target list node.");
      _quit_code = ERR_UNCONFIGURED;
      goto on_error;
    }
  }

  _update_option_nodes(_target_list_node);

  emit_signal(SIGNAL_ON_READY, this);
  _is_ready = true;
} // enclosure closing

  return;
  

  on_error:{}
  SceneTree* _tree = get_tree();
  trigger_generic_error_message([_tree, _quit_code](){
    _tree->quit(_quit_code);
  });
}

void OptionListMenu::_process(double delta){
  Engine* _engine = Engine::get_singleton();
  if(_engine->is_editor_hint())
    return;

  _update_list.update();
}


NodePath OptionListMenu::get_target_list_node() const{
  return _target_list_node_path;
}

void OptionListMenu::set_target_list_node(const NodePath& path){
  _target_list_node_path = path;
}


NodePath OptionListMenu::get_value_control_path(const String& key, Node* relative_node) const{
  auto _iter = _option_lists.find(key);
  if(_iter == _option_lists.end()){
    GameUtils::Logger::print_warn_static(gd_format_str("[OptionListMenu] Cannot get path of OptionValueControl node of key '{0}'.", key));
    return NodePath();
  }

  if(relative_node)
    return relative_node->get_path_to(_iter->second);
  
  return _iter->second->get_path();
}


OptionValueControl* OptionListMenu::get_value_control_node(const String& key) const{
  auto _iter = _option_lists.find(key);
  if(_iter == _option_lists.end()){
    GameUtils::Logger::print_warn_static(gd_format_str("[OptionListMenu] Cannot get OptionValueControl node of key '{0}'.", key));
    return NULL;
  }

  return _iter->second;
}


void OptionListMenu::set_value_data(const String& key, const Variant& value){
  auto _iter = _option_lists.find(key);
  if(_iter == _option_lists.end()){
    GameUtils::Logger::print_warn_static(gd_format_str("[OptionListMenu] Cannot set value of option of key '{0}'.", key));
    return;
  }

  if(!_is_ready){
    _update_list.append(Callable(this, "set_value_data").bind(key, value));
    return;
  }

  _iter->second->set_option_value(value);
}

Variant OptionListMenu::get_value_data(const String& key) const{
  auto _iter = _option_lists.find(key);
  if(_iter == _option_lists.end()){
    GameUtils::Logger::print_warn_static(gd_format_str("[OptionListMenu] Cannot get value of option of key '{0}'.", key));
    return Variant();
  }

  return _iter->second->get_option_value();
}


Array OptionListMenu::get_option_keys() const{
  Array _res;
  for(auto _pair: _option_lists)
    _res.append(_pair.first);

  return _res;
}
#include "application_metadata.h"
#include "window_updater.h"
#include "error_trigger.h"
#include "logger.h"

#include "godot_cpp/classes/engine.hpp"
#include "godot_cpp/classes/scene_tree.hpp"


using namespace ErrorTrigger;
using namespace godot;


void WindowUpdater::_bind_methods(){
  ClassDB::bind_method(D_METHOD("_on_data_loaded"), &WindowUpdater::_on_data_loaded);
  ClassDB::bind_method(D_METHOD("_on_window_size_changed", "window_node"), &WindowUpdater::_on_window_size_changed);

  ADD_PROPERTY(PropertyInfo(Variant::VECTOR2I, "minimal_window_size"), "set_minimal_window_size", "get_minimal_window_size");
}


void WindowUpdater::_on_data_loaded(){
  Window* _root_window = get_tree()->get_root();
  
  Variant _window_size = _data_get_function.call("last_window_size");
  Variant _window_pos = _data_get_function.call("last_window_position");
  Variant _window_mode = _data_get_function.call("last_window_mode");

  if(_window_size.get_type() != Variant::NIL)
    _root_window->set_size(_window_size);

  if(_window_pos.get_type() != Variant::NIL){
    _last_window_position = _window_pos;
    _root_window->set_position(_window_pos);
  }

  if(_window_pos.get_type() != Variant::NIL){
    _last_window_mode = _window_mode;
    _root_window->set_mode((Window::Mode)(int)_window_mode);
  }
}

void WindowUpdater::_on_window_size_changed(Window* window){
  _data_set_function.call("last_window_size", window->get_size());
  _data_set_function.call("last_window_position", window->get_position());
  _data_set_function.call("last_window_mode", window->get_mode());
}


void WindowUpdater::_ready(){
  Engine* _engine = Engine::get_singleton();
  if(_engine->is_editor_hint())
    return;

  int _quit_code = 0;

{ // enclosure for using gotos
  ApplicationMetadata* _metadata = get_node<ApplicationMetadata>("/root/GlobalApplicationMetadata");
  if(!_metadata){
    GameUtils::Logger::print_err_static("[WindowUpdater] Cannot get Application Metadata.");
    _quit_code = ERR_UNCONFIGURED;
    goto on_error;
  }

  Window* _root_window = get_tree()->get_root();
  _root_window->connect("size_changed", Callable(this, "_on_window_size_changed").bind(_root_window));

  _data_set_function = Callable(_metadata, "set_data");
  _data_get_function = Callable(_metadata, "get_data");
  _metadata->connect(PersistanceNode::s_data_loaded, Callable(this, "_on_data_loaded"));
  if(_metadata->is_data_loaded())
    _on_data_loaded();
} // enclosure closing

  return;


  on_error:{
    trigger_generic_error_message();   
    get_tree()->quit(_quit_code);
  }
}

void WindowUpdater::_process(double delta){
  Engine* _engine = Engine::get_singleton();
  if(_engine->is_editor_hint())
    return;

  Window* _root_window = get_tree()->get_root();
  
  Vector2i _window_pos = _root_window->get_position();
  if(_last_window_position != _window_pos){
    _last_window_position = _window_pos;
    _data_set_function.call("last_window_position", _last_window_position);
  }

  int _window_mode = _root_window->get_mode();
  if(_last_window_mode != _window_mode){
    _last_window_mode = _window_mode;
    _data_set_function.call("last_window_mode", _last_window_mode);
  }
}
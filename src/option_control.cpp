#include "common_event.h"
#include "error_trigger.h"
#include "gdutils.h"
#include "logger.h"
#include "option_control.h"
#include "persistance_node.h"
#include "strutil.h"

#include "godot_cpp/classes/engine.hpp"
#include "godot_cpp/classes/file_access.hpp"
#include "godot_cpp/classes/scene_tree.hpp"
#include "godot_cpp/variant/array.hpp"


using namespace gdutils;
using namespace godot;


const char* OptionControl::gvar_object_node_path = "option_control_path";

const char* OptionControl::s_value_set = "value_set";

const char* _show_animation = "slide_animation";
const char* _hide_animation = "slide_animation_hide";


void OptionControl::_bind_methods(){
  ClassDB::bind_method(D_METHOD("_on_value_set", "key", "value"), &OptionControl::_on_value_set);
  ClassDB::bind_method(D_METHOD("_on_option_button_pressed"), &OptionControl::_on_option_button_pressed);
  ClassDB::bind_method(D_METHOD("_on_option_focus_exited"), &OptionControl::_on_option_focus_exited);
  ClassDB::bind_method(D_METHOD("_on_option_list_menu_ready", "node"), &OptionControl::_on_option_list_menu_ready);
  ClassDB::bind_method(D_METHOD("_on_config_loaded"), &OptionControl::_on_config_loaded);

  ClassDB::bind_method(D_METHOD("set_logo_settings_image", "image"), &OptionControl::set_logo_settings_image);
  ClassDB::bind_method(D_METHOD("get_logo_settings_image"), &OptionControl::get_logo_settings_image);

  ClassDB::bind_method(D_METHOD("set_logo_settings_close_image", "image"), &OptionControl::set_logo_settings_close_image);
  ClassDB::bind_method(D_METHOD("get_logo_settings_close_image"), &OptionControl::get_logo_settings_close_image);

  ClassDB::bind_method(D_METHOD("set_animation_player", "path"), &OptionControl::set_animation_player);
  ClassDB::bind_method(D_METHOD("get_animation_player"), &OptionControl::get_animation_player);

  ClassDB::bind_method(D_METHOD("set_settings_button", "path"), &OptionControl::set_settings_button);
  ClassDB::bind_method(D_METHOD("get_settings_button"), &OptionControl::get_settings_button);

  ClassDB::bind_method(D_METHOD("set_settings_unfocus_area", "path"), &OptionControl::set_settings_unfocus_area);
  ClassDB::bind_method(D_METHOD("get_settings_unfocus_area"), &OptionControl::get_settings_unfocus_area);

  ClassDB::bind_method(D_METHOD("set_option_control_path", "path"), &OptionControl::set_option_control_path);
  ClassDB::bind_method(D_METHOD("get_option_control_path"), &OptionControl::get_option_control_path);

  ADD_SIGNAL(MethodInfo(OptionControl::s_value_set, PropertyInfo(Variant::STRING, "key"), PropertyInfo(Variant::NIL, "value")));

  ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "settings_logo", PROPERTY_HINT_RESOURCE_TYPE, "Texture"), "set_logo_settings_image", "get_logo_settings_image");
  ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "close_logo", PROPERTY_HINT_RESOURCE_TYPE, "Texture"), "set_logo_settings_close_image", "get_logo_settings_close_image");
  ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "animation_player"), "set_animation_player", "get_animation_player");
  ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "settings_button"), "set_settings_button", "get_settings_button");
  ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "settings_unfocus_area"), "set_settings_unfocus_area", "get_settings_unfocus_area");
  ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "option_control"), "set_option_control_path", "get_option_control_path");
}


void OptionControl::_on_value_set(const String& key, const Variant& value){
  set_option_value(key, value, false);
}


void OptionControl::_on_option_button_pressed(){
  if(_is_showing)
    _hide_option_ui();
  else
    _show_option_ui();
}

void OptionControl::_on_option_focus_exited(){
  _hide_option_ui();
}


void OptionControl::_on_option_list_menu_ready(Node* node){
  _update_option_ui();
}


void OptionControl::_on_config_loaded(){
  _update_option_ui();
}


void OptionControl::_update_option_ui(){
  Array _key_list = _option_menu->get_option_keys();
  for(int i = 0; i < _key_list.size(); i++){
    Variant _key = _key_list[i];
    Variant _value = _data_get_function.call(_key);

    _option_menu->set_value_data(_key, _value);
  }
}


void OptionControl::_play_show_option_ui(bool hide){
  double _blend = -1;
  if(_animation_player->is_playing()){
    _blend =
      _animation_player->get_current_animation_length() -
      _animation_player->get_current_animation_position();
  }

  if(hide)
    _animation_player->play(_hide_animation, _blend);
  else
    _animation_player->play(_show_animation, _blend);
}

void OptionControl::_show_option_ui(){
  _play_show_option_ui(false);
  _settings_button->set_button_icon(_logo_settings_close_image);
  _is_showing = true;
}

void OptionControl::_hide_option_ui(){
  _play_show_option_ui(true);
  _settings_button->set_button_icon(_logo_settings_image);
  _is_showing = false;
}


void OptionControl::_ready(){
  Engine* _engine = Engine::get_singleton();
  if(_engine->is_editor_hint())
    return;

  int _quit_code = 0;

{ // enclosure for using goto
  _gvariables = get_node<GlobalVariables>(GlobalVariables::singleton_path);
  if(!_gvariables){
    GameUtils::Logger::print_err_static("[OptionControl] Cannot get GlobalVariables singleton object. (Is it not configured?)");
    _quit_code = ERR_UNCONFIGURED;
    goto on_error_label;
  }

  _animation_player = get_node<AnimationPlayer>(_animation_player_path);
  if(!_animation_player){
    GameUtils::Logger::print_err_static("[OptionControl] Cannot get AnimationPlayer.");
    _quit_code = ERR_UNCONFIGURED;
    goto on_error_label;
  }

  _settings_button = get_node<Button>(_settings_button_path);
  if(!_settings_button){
    GameUtils::Logger::print_err_static("[OptionControl] Cannot get Show Button for Option UI.");
    _quit_code = ERR_UNCONFIGURED;
    goto on_error_label;
  }

  _settings_unfocus_area = get_node<Control>(_settings_unfocus_area_path);
  if(!_settings_unfocus_area){
    GameUtils::Logger::print_err_static("[OptionControl] Cannot Focus area for unfocus state.");
    _quit_code = ERR_UNCONFIGURED;
    goto on_error_label;
  }

  Control* _option_control = get_node<Control>(_option_control_path);
  if(!_option_control){
    GameUtils::Logger::print_err_static("[OptionControl] Cannot get option control path.");
    _quit_code = ERR_UNCONFIGURED;
    goto on_error_label;
  }

  _option_menu = find_any_node<OptionListMenu>(_option_control, true);
  if(!_option_menu){
    GameUtils::Logger::print_err_static("[OptionControl] Cannot get OptionListMenu.");
    _quit_code = ERR_UNCONFIGURED;
    goto on_error_label;
  }

  PersistanceNode* _data_node = get_node<PersistanceNode>("/root/GlobalApplicationConfig");
  if(!_data_node){
    GameUtils::Logger::print_err_static("[OptionControl] Cannot get Application Config Data.");
    goto on_error_label;
  }

  _data_set_function = Callable(_data_node, "set_data");
  _data_get_function = Callable(_data_node, "get_data");
  _data_node->connect(PersistanceNode::s_data_loaded, Callable(this, "_on_config_loaded"));
  if(_data_node->is_data_loaded())
    _on_config_loaded();

  _gvariables->set_global_value(OptionControl::gvar_object_node_path, get_path());

  _option_menu->connect(OptionListMenu::s_value_set, Callable(this, "_on_value_set"));
  _option_menu->connect(SIGNAL_ON_READY, Callable(this, "_on_option_list_menu_ready"));

  _settings_button->connect("pressed", Callable(this, "_on_option_button_pressed"));
  _settings_unfocus_area->connect("focus_entered", Callable(this, "_on_option_focus_exited"));
} // enclosure closing

  return;


  on_error_label:{
    ErrorTrigger::trigger_generic_error_message();
    get_tree()->quit(_quit_code);
  }
}


void OptionControl::set_option_value(const String& key, const Variant& value, bool update_ui){
  _data_set_function.call(key, value);
  if(update_ui)
    _option_menu->set_value_data(key, value);
  
  emit_signal(s_value_set, key, value);
}

Variant OptionControl::get_option_value(const String& key){
  return _data_get_function.call(key);
}


void OptionControl::set_logo_settings_image(Ref<Texture> image){
  _logo_settings_image = image;
}

Ref<Texture> OptionControl::get_logo_settings_image() const{
  return _logo_settings_image;
}


void OptionControl::set_logo_settings_close_image(Ref<Texture> image){
  _logo_settings_close_image = image;
}

Ref<Texture> OptionControl::get_logo_settings_close_image() const{
  return _logo_settings_close_image;
}


void OptionControl::set_animation_player(const NodePath& path){
  _animation_player_path = path;
}

NodePath OptionControl::get_animation_player() const{
  return _animation_player_path;
}


void OptionControl::set_settings_button(const NodePath& path){
  _settings_button_path = path;
}

NodePath OptionControl::get_settings_button() const{
  return _settings_button_path;
}


void OptionControl::set_settings_unfocus_area(const NodePath& path){
  _settings_unfocus_area_path = path;
}

NodePath OptionControl::get_settings_unfocus_area() const{
  return _settings_unfocus_area_path;
}


void OptionControl::set_option_control_path(const NodePath& path){
  _option_control_path = path;
}

NodePath OptionControl::get_option_control_path() const{
  return _option_control_path;
}
#include "application_config_data.h"
#include "dllutil.h"
#include "error_trigger.h"
#include "logger.h"
#include "node_utils.h"
#include "ui_control.h"

#include "godot_cpp/classes/engine.hpp"
#include "godot_cpp/classes/label.hpp"
#include "godot_cpp/classes/scene_tree.hpp"
#include "godot_cpp/classes/window.hpp"

#include "functional"
#include "map"


using namespace ErrorTrigger;
using namespace godot;
using namespace dynamic_library::util;


void _code_deinit();
static destructor_helper _dh(_code_deinit);

typedef void (UIControl::*class_func)();
static std::map<String, class_func>* _on_data_changed_lookup = NULL;


void UIControl::_code_init(){
  if(!_on_data_changed_lookup){
    _on_data_changed_lookup = new std::map<String, class_func>{
      {"code_window_font", &UIControl::_on_data_changed_code_window_font},
      {"inspector_font", &UIControl::_on_data_changed_inspector_font},
      {"console_window_font", &UIControl::_on_data_changed_console_window_font},
      {"ui_scale", &UIControl::_on_data_changed_ui_scale}
    };
  }
}

void _code_deinit(){
  if(_on_data_changed_lookup){
    delete _on_data_changed_lookup;
    _on_data_changed_lookup = NULL;
  }
}


void UIControl::_bind_methods(){
  ClassDB::bind_method(D_METHOD("_on_data_changed", "key"), &UIControl::_on_data_changed);
  ClassDB::bind_method(D_METHOD("_on_target_min_size_changed", "target_node"), &UIControl::_on_target_min_size_changed);
  ClassDB::bind_method(D_METHOD("_on_data_loaded"), &UIControl::_on_data_loaded);

  ClassDB::bind_method(D_METHOD("set_target_min_size_node_path", "path"), &UIControl::set_target_min_size_node_path);
  ClassDB::bind_method(D_METHOD("get_target_min_size_node_path"), &UIControl::get_target_min_size_node_path);

  ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "target_min_size_node"), "set_target_min_size_node_path", "get_target_min_size_node_path");
}


void UIControl::_on_data_changed(const String& key){
  auto _iter = _on_data_changed_lookup->find(key);
  if(_iter == _on_data_changed_lookup->end())
    return;

  (this->*_iter->second)();
}

void UIControl::_on_data_changed_code_window_font(){
  Variant _font_size = _config_get_function.call("code_window_font");
  if(_font_size.get_type() == Variant::NIL)
    return;

  _ginvoker->invoke("code_window_group", "add_theme_font_size_override", "font_size", _font_size);
}

void UIControl::_on_data_changed_inspector_font(){
  Variant _font_size = _config_get_function.call("inspector_font");
  if(_font_size.get_type() == Variant::NIL)
    return;

  _ginvoker->invoke("inspector_group", "add_theme_font_size_override", "font_size", _font_size);
}

void UIControl::_on_data_changed_console_window_font(){
  Variant _font_size = _config_get_function.call("console_window_font");
  if(_font_size.get_type() == Variant::NIL)
    return;

  _ginvoker->invoke("console_window_group", "add_theme_font_size_override", "font_size", _font_size);
  _ginvoker->invoke("console_window_group", "add_theme_font_size_override", "bold_italics_font_size", _font_size);
  _ginvoker->invoke("console_window_group", "add_theme_font_size_override", "italics_font_size", _font_size);
  _ginvoker->invoke("console_window_group", "add_theme_font_size_override", "mono_font_size", _font_size);
  _ginvoker->invoke("console_window_group", "add_theme_font_size_override", "normal_font_size", _font_size);
  _ginvoker->invoke("console_window_group", "add_theme_font_size_override", "bold_font_size", _font_size);
}

void UIControl::_on_data_changed_ui_scale(){
  Variant _ui_scale = _config_get_function.call("ui_scale");
  if(_ui_scale.get_type() == Variant::NIL)
    return;

  int _ui_scale_int = _ui_scale;
  get_window()->set_content_scale_factor((float)_ui_scale_int/100);
  _update_window_min_size();
}


void UIControl::_on_target_min_size_changed(Node* target_node){
  if(!target_node->is_class(Control::get_class_static()))
    return;

  Control* _cnode = (Control*)target_node;
  _target_min_size = _cnode->get_minimum_size();
  _update_window_min_size();
}


void UIControl::_on_data_loaded(){
  _on_data_changed_code_window_font();
  _on_data_changed_inspector_font();
  _on_data_changed_console_window_font();
  _on_data_changed_ui_scale();
}


void UIControl::_update_window_min_size(){
  Window* _window = get_window();
  _window->set_min_size(_target_min_size * _window->get_content_scale_factor());
}


void UIControl::_ready(){
  _code_init();

  Engine* _engine = Engine::get_singleton();
  if(_engine->is_editor_hint())
    return;

  int _quit_code = 0;

{ // enclosure for using gotos
  _ginvoker = get_any_node<GroupInvoker>(this);
  if(!_ginvoker){
    GameUtils::Logger::print_err_static("[UIControl] Cannot get GroupInvoker in child.");
    _quit_code = ERR_UNCONFIGURED;
    goto on_error;
  }

  ApplicationConfigData* _config_data = get_node<ApplicationConfigData>("/root/GlobalApplicationConfig");
  if(!_config_data){
    GameUtils::Logger::print_err_static("[UIControl] Cannot get global ApplicationConfigData.");
    _quit_code = ERR_UNCONFIGURED;
    goto on_error;
  }

  Control* _target_min_size_node = get_node<Control>(_target_min_size_node_path);
  if(!_target_min_size_node_path.is_empty() && !_target_min_size_node)
    GameUtils::Logger::print_warn_static("[UIControl] Target node for base minimum size is not a type of Control or other same inheriting objects.");

  if(_target_min_size_node){
    _target_min_size_node->connect("minimum_size_changed", Callable(this, "_on_target_min_size_changed").bind(_target_min_size_node));
    _target_min_size = _target_min_size_node->get_minimum_size();  
  }

  _config_get_function = Callable(_config_data, "get_data");
  _config_data->connect(ApplicationConfigData::s_data_changed, Callable(this, "_on_data_changed"));
  _config_data->connect(ApplicationConfigData::s_data_loaded, Callable(this, "_on_data_loaded"));
  if(_config_data->is_data_loaded())
    _on_data_loaded();
} // enclosure closing

  return;


  on_error:{}
  SceneTree* _tree = get_tree();
  trigger_generic_error_message([_tree, _quit_code](){
    _tree->quit(_quit_code);
  });
}


void UIControl::set_target_min_size_node_path(const NodePath& path){
  _target_min_size_node_path = path;
}

NodePath UIControl::get_target_min_size_node_path() const{
  return _target_min_size_node_path;
}
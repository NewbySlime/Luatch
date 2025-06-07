#include "dialog_window.h"
#include "error_trigger.h"
#include "logger.h"
#include "node_utils.h"
#include "strutil.h"

#include "godot_cpp/classes/button.hpp"
#include "godot_cpp/classes/engine.hpp"
#include "godot_cpp/classes/scene_tree.hpp"


using namespace ErrorTrigger;
using namespace godot;


const char* DialogWindow::s_on_button_pressed = "choice_pressed";


void DialogWindow::_bind_methods(){
  ClassDB::bind_method(D_METHOD("_on_pivot_node_resized"), &DialogWindow::_on_pivot_node_resized);
  ClassDB::bind_method(D_METHOD("_on_child_order_changed"), &DialogWindow::_on_child_order_changed);
  ClassDB::bind_method(D_METHOD("_on_button_pressed", "key"), &DialogWindow::_on_button_pressed);
  ClassDB::bind_method(D_METHOD("_on_popup"), &DialogWindow::_on_popup);

  ADD_SIGNAL(MethodInfo(DialogWindow::s_on_button_pressed, PropertyInfo(Variant::STRING, "key")));
}


DialogWindow::~DialogWindow(){
  clear_choice_list();
}


void DialogWindow::_update_size(){
  set_size(_ui_pivot_node->get_size());
}

void DialogWindow::_update_pivot_node(){
  if(_ui_pivot_node)
    _unbind_pivot_node(_ui_pivot_node);

  Control* _target_child = NULL;
  for(size_t i = 0; i < get_child_count(); i++){
    Node* _target_child_node = get_child(i);
    if(!_target_child_node->is_class(Control::get_class_static()))
      continue;

    _target_child = (Control*)_target_child_node;
  }

  _ui_pivot_node = _target_child;
  if(_ui_pivot_node)
    _bind_pivot_node(_target_child);

  _text_node = NULL;
  _button_pivot_node = NULL;

  if(!_ui_pivot_node)
    return;

  _text_node = _ui_pivot_node->get_node<RichTextLabel>("%TextControl");
  if(!_text_node){
    GameUtils::Logger::print_err_static("[DialogWindow] Cannot get text control.");
    return;
  }

  _button_pivot_node = _ui_pivot_node->get_node<Control>("%ButtonPivot");
  if(!_button_pivot_node){
    GameUtils::Logger::print_err_static("[DialogWindow] Cannot get button pivot.");
    return;
  }
}


void DialogWindow::_on_pivot_node_resized(){
  _update_size();
}

void DialogWindow::_on_child_order_changed(){
  _update_pivot_node();
}

void DialogWindow::_on_button_pressed(const String& key){
  auto _iter = _choice_map.find(key);
  if(_iter != _choice_map.end()){
    if(_iter->second->hide_on_choosen)
      hide();

    if(_iter->second->pressed_callback.is_valid())
      _iter->second->pressed_callback.call();
  }

  emit_signal(s_on_button_pressed, key);
}

void DialogWindow::_on_popup(){
  if(_text_node)
    _text_node->set_text(_text_data);

  if(_button_pivot_node){
    clear_child((Node*)_button_pivot_node);
    
    for(ChoiceData* _cd: _choice_list){
      Button* _button = memnew(Button);
      _button->set_text(_cd->readable_name);
      _button_pivot_node->add_child(_button);

      _button->connect("pressed", Callable(this, "_on_button_pressed").bind(_cd->key));
    }
  }
}


void DialogWindow::_bind_pivot_node(Control* node){
  node->connect("resized", Callable(this, "_on_pivot_node_resized"));
}

void DialogWindow::_unbind_pivot_node(Control* node){
  node->disconnect("resized", Callable(this, "_on_pivot_node_resized"));
}


void DialogWindow::_ready(){
  Engine* _engine = Engine::get_singleton();
  if(_engine->is_editor_hint())
    return;

  int _quit_code;

{ // enclosure for using gotos
  connect("child_order_changed", Callable(this, "_on_child_order_changed"));
  connect("about_to_popup", Callable(this, "_on_popup"));

  _update_pivot_node();
} // enclosure closing

  return; 


  on_error:{}
  trigger_generic_error_message();
  get_tree()->quit(_quit_code);
}


void DialogWindow::set_text(const String& text){
  _text_data = text;
}

String DialogWindow::get_text() const{
  return _text_data;
}


void DialogWindow::add_new_choice(const ChoiceData& data){
  auto _iter = _choice_map.find(data.key);
  if(_iter != _choice_map.end()){
    GameUtils::Logger::print_warn_static(gd_format_str("[DialogWindow] '{0}' choice already exists.", data.key));
    return;
  }

  ChoiceData* _choice = new ChoiceData();
  *_choice = data;

  _choice_map[data.key] = _choice;
  _choice_list.push_back(_choice);
}

void DialogWindow::clear_choice_list(){
  for(ChoiceData* _cd: _choice_list)
    delete _cd;

  _choice_list.clear();
  _choice_map.clear();
}


size_t DialogWindow::get_choice_data_count() const{
  return _choice_list.size();
}
 
const DialogWindow::ChoiceData* DialogWindow::get_choice_data(size_t idx) const{
  return _choice_list[idx];
}

const DialogWindow::ChoiceData* DialogWindow::get_choice_data(const String& key) const{
  auto _iter = _choice_map.find(key);
  if(_iter == _choice_map.end())
    return NULL;

  return _iter->second;
}
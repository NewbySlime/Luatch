#include "dialog_window.h"
#include "error_trigger.h"
#include "error_trigger_initializer.h"
#include "instance_database.h"
#include "logger.h"
#include "node_utils.h"

#include "godot_cpp/classes/engine.hpp"
#include "godot_cpp/classes/scene_tree.hpp"


using namespace ErrorTrigger;
using namespace godot;


static ErrorTriggerInitializer* _singleton_obj = NULL;


static void _trigger_message_func(const char* error_message){
  if(!_singleton_obj){
    GameUtils::Logger::print_err_static("[ErrorTriggerInitializer] No instance of the singleton object is found?");
    return;
  }

  _singleton_obj->trigger_error(error_message);
}

static void _trigger_message_gdcallback_func(const char* error_message, Callable cb){
  if(!_singleton_obj){
    GameUtils::Logger::print_err_static("[ErrorTriggerInitializer] No instance of the singleton object is found?");
    return;
  }

  _singleton_obj->trigger_error(error_message, cb);
}

static void _trigger_message_callback_func(const char* error_message, trigger_error_callback cb){
  if(!_singleton_obj){
    GameUtils::Logger::print_err_static("[ErrorTriggerInitializer] No instance of the singleton object is found?");
    return;
  }

  _singleton_obj->trigger_error(error_message, cb);
}


void ErrorTriggerInitializer::_bind_methods(){
  ClassDB::bind_method(D_METHOD("_trigger_error_safe", "window"), &ErrorTriggerInitializer::_trigger_error_safe);
  ClassDB::bind_method(D_METHOD("_on_error_trigger_pressed", "key", "trigger_node"), &ErrorTriggerInitializer::_on_error_trigger_pressed);
}


void ErrorTriggerInitializer::_trigger_error(const char* message){
    InstanceDatabase* _instance_db = get_node<InstanceDatabase>("/root/GlobalInstanceDatabase");
  if(!_instance_db){
    GameUtils::Logger::print_err_static("[ErrorTriggerInitializer] Cannot get instance database.");
    return;
  }
  
  Ref<PackedScene> _instance_object = _instance_db->get_instance("DialogWindow");
  if(!_instance_object.is_valid()){
    GameUtils::Logger::print_err_static("[ErrorTriggerInitializer] Cannot get instance object for DialogWindow.");
    return;
  }

  Node* _dialog_window_node = _instance_object->instantiate();
  DialogWindow* _dialog_window = get_any_node<DialogWindow>(_dialog_window_node, false, true);
  if(!_dialog_window){
    GameUtils::Logger::print_err_static("[ErrorTriggerInitializer] Cannot get DialogWindow object in instantiated node.");
    return;
  }

  _dialog_window->set_title("Error Occurred");
  _dialog_window->set_text(message);

  DialogWindow::ChoiceData _choice_data;
    _choice_data.key = "ok_button";
    _choice_data.readable_name = "Ok";
    _choice_data.hide_on_choosen = true;
  _dialog_window->add_new_choice(_choice_data);
  _dialog_window->connect(DialogWindow::s_on_button_pressed, Callable(this, "_on_error_trigger_pressed").bind(_dialog_window));

  call_deferred("_trigger_error_safe", _dialog_window);
}

void ErrorTriggerInitializer::_trigger_error_safe(DialogWindow* window){
  get_tree()->get_root()->add_child(window);
  window->hide();
  window->set_force_native(true);
  window->popup();
}


void ErrorTriggerInitializer::_on_error_trigger_pressed(const String& key, Node* trigger_node){
  trigger_node->queue_free();

  if(_error_callback_gd.is_valid())
    _error_callback_gd.call();
  
  if(_error_callback_std)
    _error_callback_std();
}


void ErrorTriggerInitializer::_ready(){
#if !((_WIN64) || (_WIN32))
  Engine* _engine = Engine::get_singleton();
  if(_engine->is_editor_hint())
    return;

  if(_singleton_obj){
    GameUtils::Logger::print_warn_static("[ErrorTriggerInitializer] Another instance of this object exists.");
    return;
  }

  _singleton_obj = this;
  set_trigger_message_callback(_trigger_message_func);
  set_trigger_message_callback(_trigger_message_gdcallback_func);
  set_trigger_message_callback(_trigger_message_callback_func);
#endif
}


void ErrorTriggerInitializer::trigger_error(const char* message){
  _error_callback_gd = Callable();
  _error_callback_std = NULL;
  _trigger_error(message);
}

void ErrorTriggerInitializer::trigger_error(const char* message, Callable cb){
  _error_callback_std = NULL;
  _error_callback_gd = cb;
  _trigger_error(message);
}

void ErrorTriggerInitializer::trigger_error(const char* message, trigger_error_callback cb){
  _error_callback_gd = Callable();
  _error_callback_std = cb;
  _trigger_error(message);
}
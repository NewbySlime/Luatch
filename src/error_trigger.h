#ifndef ERROR_TRIGGER_HEADER
#define ERROR_TRIGGER_HEADER

#include "godot_cpp/variant/variant.hpp"

#include "functional"


// Since this trigger are for Windows platform, another platform should register a trigger function via set_trigger_message_callback function.
namespace ErrorTrigger{
  typedef std::function<void()> trigger_error_callback;
  void trigger_generic_error_message();
  void trigger_generic_error_message(godot::Callable callback);
  void trigger_generic_error_message(trigger_error_callback callback);
  void trigger_error_message(const char* error_msg);
  void trigger_error_message(const char* error_msg, godot::Callable callback);
  void trigger_error_message(const char* error_msg, trigger_error_callback callback);

  typedef void(*trigger_message_func)(const char* error_message);
  typedef void(*trigger_message_gdcallback_func)(const char* error_message, godot::Callable callback);
  typedef void(*trigger_message_callback_func)(const char* error_message, trigger_error_callback callback);
  void set_trigger_message_callback(trigger_message_func callback_func);
  void set_trigger_message_callback(trigger_message_gdcallback_func callback_func);
  void set_trigger_message_callback(trigger_message_callback_func callback_func);
}

#endif
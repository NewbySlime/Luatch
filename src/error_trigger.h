#ifndef ERROR_TRIGGER_HEADER
#define ERROR_TRIGGER_HEADER


// Since this trigger uses Windows 
namespace ErrorTrigger{
  void trigger_generic_error_message();
  void trigger_error_message(const char* error_msg);

  typedef void(*trigger_message_func)(const char* error_message);
  void set_trigger_message_callback(trigger_message_func callback_func);
}

#endif
#include "error_trigger.h"
#include "stddef.h"

#if (_WIN64) || (_WIN32)
#include "Windows.h"
#endif


using namespace ErrorTrigger;


const char* _title_message = "Debugger Error";
const char* _generic_error_msg = "Something went wrong. Check logs for more information.";


static trigger_message_func _trigger_callback = NULL;


void ErrorTrigger::trigger_generic_error_message(){
  trigger_error_message(_generic_error_msg);
}

void ErrorTrigger::trigger_error_message(const char* error_msg){
  if(_trigger_callback){
    _trigger_callback(error_msg);
    return;
  }

#if (_WIN64) || (_WIN32)
  MessageBoxA(
    NULL,
    error_msg,
    _title_message,
    MB_OK | MB_ICONERROR
  );
#endif
}


void ErrorTrigger::set_trigger_message_callback(trigger_message_func callback_func){
  _trigger_callback = callback_func;
}
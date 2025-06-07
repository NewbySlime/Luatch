#include "error_trigger.h"
#include "stddef.h"

#if (_WIN64) || (_WIN32)
#include "Windows.h"
#endif


using namespace ErrorTrigger;
using namespace godot;


const char* _title_message = "Debugger Error";
const char* _generic_error_msg = "Something went wrong. Check logs for more information.";


static trigger_message_func _trigger_callback = NULL;
static trigger_message_gdcallback_func _trigger_gdcallback = NULL;
static trigger_message_callback_func _trigger_stdcallback = NULL;



// This function expected to be blocked.
static void _trigger_platform_error_message(const char* error_msg){
#if (_WIN64) || (_WIN32)
  MessageBoxA(
    NULL,
    error_msg,
    _title_message,
    MB_OK | MB_ICONERROR
  );
#endif
}


void ErrorTrigger::trigger_generic_error_message(){
  trigger_error_message(_generic_error_msg);
}

void ErrorTrigger::trigger_generic_error_message(Callable cb){
  trigger_error_message(_generic_error_msg, cb);
}

void ErrorTrigger::trigger_generic_error_message(trigger_error_callback cb){
  trigger_error_message(_generic_error_msg, cb);
}

void ErrorTrigger::trigger_error_message(const char* error_msg){
  if(_trigger_callback){
    _trigger_callback(error_msg);
    return;
  }

  _trigger_platform_error_message(error_msg);
}

void ErrorTrigger::trigger_error_message(const char* error_msg, Callable cb){
  if(_trigger_gdcallback){
    _trigger_gdcallback(error_msg, cb);
    return;
  }

  _trigger_platform_error_message(error_msg);
  cb.call();
}

void ErrorTrigger::trigger_error_message(const char* error_msg, trigger_error_callback cb){
  if(_trigger_stdcallback){
    _trigger_stdcallback(error_msg, cb);
    return;
  }

  _trigger_platform_error_message(error_msg);
  cb();
}


void ErrorTrigger::set_trigger_message_callback(trigger_message_func callback_func){
  _trigger_callback = callback_func;
}

void ErrorTrigger::set_trigger_message_callback(trigger_message_gdcallback_func callback_func){
  _trigger_gdcallback = callback_func;
}

void ErrorTrigger::set_trigger_message_callback(trigger_message_callback_func callback_func){
  _trigger_stdcallback = callback_func;
}
#include "debug_log.h"

#ifdef BUILD_VERBOSE
static log_verbose_func_godot log_func_gd = NULL;
static log_verbose_func_godot log_warn_func_gd = NULL;
static log_verbose_func_godot log_err_func_gd = NULL;

static log_verbose_func_std log_func_std = NULL;
static log_verbose_func_std log_warn_func_std = NULL;
static log_verbose_func_std log_err_func_std = NULL;


void set_log_verbose_func(log_verbose_func_godot func, log_verbose_func_godot warn_func, log_verbose_func_godot err_func){
  log_func_gd = func;
  log_warn_func_gd = warn_func;
  log_err_func_gd = err_func;
}

void set_log_verbose_func(log_verbose_func_std func, log_verbose_func_std warn_func, log_verbose_func_std err_func){
  log_func_std = func;
  log_warn_func_std = warn_func;
  log_err_func_std = err_func;
}


void log_verbose(const godot::String& str){
  if(log_func_gd)
    log_func_gd(str);
}

void log_warn_verbose(const godot::String& str){
  if(log_warn_func_gd)
    log_warn_func_gd(str);
}

void log_err_verbose(const godot::String& str){
  if(log_err_func_gd)
    log_err_func_gd(str);
}


void log_verbose(const std::string& str){
  if(log_func_std)
    log_func_std(str);
}

void log_warn_verbose(const std::string& str){
  if(log_warn_func_std)
    log_warn_func_std(str);
}

void log_err_verbose(const std::string& str){
  if(log_err_func_std)
    log_err_func_std(str);
}
#endif
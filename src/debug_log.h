#ifndef DEBUG_LOG_HEADER
#define DEBUG_LOG_HEADER

#include "string"
#include "godot_cpp/variant/string.hpp"


// NOTE: Only use the macros, so when not created with "Verbose" flag, the argument will be discarded.

#ifdef BUILD_VERBOSE
#define PRINT_LOG_VERBOSE(str) log_verbose(str)
#define PRINT_LOG_WARN_VERBOSE(str) log_warn_verbose(str)
#define PRINT_LOG_ERR_VERBOSE(str) log_err_verbose(str)

#define SET_LOG_VERBOSE_FUNCTION(func, warn_func, err_func) set_log_verbose_func(func, warn_func, err_func)

typedef void(*log_verbose_func_godot)(const godot::String& str);
typedef void(*log_verbose_func_std)(const std::string& str);

void set_log_verbose_func(log_verbose_func_godot func, log_verbose_func_godot warn_func, log_verbose_func_godot err_func);
void set_log_verbose_func(log_verbose_func_std func, log_verbose_func_std warn_func, log_verbose_func_std err_func);

void log_verbose(const godot::String& str);
void log_warn_verbose(const godot::String& str);
void log_err_verbose(const godot::String& str);

void log_verbose(const std::string& str);
void log_warn_verbose(const std::string& str);
void log_err_verbose(const std::string& str);
#else
#define PRINT_LOG_VERBOSE(str)
#define PRINT_LOG_WARN_VERBOSE(str)
#define PRINT_LOG_ERR_VERBOSE(str)

#define SET_LOG_VERBOSE_FUNCTION(func, warn_func, err_func)
#endif

#endif
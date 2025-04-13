#ifndef STRUTIL_HEADER
#define STRUTIL_HEADER

#include "string"

template<typename... T_args> std::string format_str(const char* str, T_args... args){
  int _str_len = snprintf(NULL, 0, str, args...);

  char* _c_str = (char*)malloc(_str_len+1);
  snprintf(_c_str, _str_len+1, str, args...);

  std::string _result = _c_str;
  free(_c_str);

  return _result;  
}

#endif


#ifndef GODOT_STRUTIL_HEADER
#define GODOT_STRUTIL_HEADER

#include "godot_cpp/variant/array.hpp"
#include "godot_cpp/variant/string.hpp"

godot::String gd_format_strv(const char* str, const godot::Variant& array);

template<typename... T_args> godot::String gd_format_str(const char* str, T_args... args){
  godot::Array _paramarr;
  ([&]{
    _paramarr.append(args);
  }(), ...);

  return gd_format_strv(str, _paramarr);
}

#endif
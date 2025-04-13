#include "strutil.h"


using namespace godot;


String gd_format_strv(const char* str, const Variant& array){
  return String(str).format(array);
}
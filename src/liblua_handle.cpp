#include "defines.h"
#include "error_trigger.h"
#include "liblua_handle.h"
#include "logger.h"
#include "strutil.h"

#include "godot_cpp/classes/engine.hpp"
#include "godot_cpp/classes/os.hpp"
#include "godot_cpp/classes/scene_tree.hpp"
#include "godot_cpp/classes/project_settings.hpp"
#include "godot_cpp/variant/string.hpp"
#include "godot_cpp/variant/utility_functions.hpp"
#include "godot_cpp/classes/scroll_container.hpp"


#if (_WIN64) || (_WIN32)
#pragma comment(lib, "user32.lib")

#define CPPLUA_LIBRARY_FNAME "CPPAPI.dll"
#elif (__linux)
#define CPPLUA_LIBRARY_FNAME "CPPAPI.so"
#endif

#define CPPLUA_LIBRARY_PATH ProjectSettings::get_singleton()->globalize_path("res://bin/" CPPLUA_LIBRARY_FNAME)


using namespace godot;


void LibLuaHandle::_bind_methods(){

}


LibLuaHandle::LibLuaHandle(){
  // don't load library here, load when on _ready()
}

LibLuaHandle::~LibLuaHandle(){
  _unload_library();
}


void LibLuaHandle::_load_library(){
  if(!_lib_store)
    _unload_library();

  int _quit_code = 0;
  std::string _library_path;{
    String __gd_str = String(CPPLUA_LIBRARY_PATH);
    _library_path.append(GDSTR_AS_PRIMITIVE(__gd_str), __gd_str.length());
  }

#if (_WIN64) || (_WIN32)
  HMODULE _library_handle = NULL;
#elif (__linux)
  void* _library_handle = NULL;
#endif

{ // enclosure for using gotos
#if (_WIN64) || (_WIN32)
  _library_handle = LoadLibraryA(_library_path.c_str());

  // Try to load in the same working directory
  if(!_library_handle)
    _library_handle = LoadLibraryA(CPPLUA_LIBRARY_FNAME);

  if(!_library_handle){
    DWORD _error_code = GetLastError();

    LPSTR _buffer_str = NULL;
    DWORD_PTR _args[] = {(DWORD_PTR)_library_path.c_str()};
    DWORD _write_len = FormatMessageA(
      FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ARGUMENT_ARRAY,
      NULL,
      _error_code,
      0,
      (LPSTR)&_buffer_str,
      0,
      (va_list*)_args
    );

    String _err_msg = gd_format_str("[LibLuaHandle] Error occurred! Code: {0} Message: {1}", Variant((uint32_t)_error_code), String(_buffer_str));
    LocalFree(_buffer_str);

    GameUtils::Logger::print_err_static(_err_msg);
    std::string _err_msg_cstr(GDSTR_AS_PRIMITIVE(_err_msg), _err_msg.length());

    ErrorTrigger::trigger_error_message(_err_msg_cstr.c_str());

    _quit_code = _error_code;
    goto on_error_label;
  }

  _lib_store = std::make_shared<LibLuaStore>(_library_handle);
#elif (__linux)
  _library_handle = dlopen(_library_path.c_str(), RTLD_NOW | RTLD_LOCAL);

  // Try to load in the same working directory
  if(!_library_handle)
    _library_handle = dlopen(CPPLUA_LIBRARY_FNAME, RTLD_NOW | RTLD_LOCAL);

  if(!_library_handle){
    GameUtils::Logger::print_err_static(gd_format_str("[LibLuaHandle] Cannot load the library. Error message: {0}", dlerror()));
    _quit_code = ERR_FILE_MISSING_DEPENDENCIES;
    goto on_error_label;
  }

  _lib_store = std::make_shared<LibLuaStore>(_library_handle);
#endif

  if(_lib_store->error_occurred()){
    GameUtils::Logger::print_err_static(gd_format_str("[LibLuaHandle] Cannot load the library. Error message: {0}", _lib_store->get_error_message().c_str()));
    _quit_code = _lib_store->get_error_code();
    goto on_error_label;
  }
} // enclosure closing

  GameUtils::Logger::print_log_static("[LibLuaHandle] Lua Debugging API loaded.");
  return;

  on_error_label:{}
#if (_WIN64) || (_WIN32)
  if(_library_handle)
    FreeLibrary(_library_handle);
#elif (__linux)
  if(_library_handle)
    dlclose(_library_handle);
#endif

  ErrorTrigger::trigger_error_message("Error occured! Cannot find needed functions for Lua Debugging API, check logs for details.");
  get_tree()->quit(_quit_code);
}

void LibLuaHandle::_unload_library(){
  _lib_store = NULL;
}


void LibLuaHandle::_ready(){
  Engine* _engine = Engine::get_singleton();
  if(_engine->is_editor_hint())
    return;

  _load_library();
}


std::shared_ptr<LibLuaStore> LibLuaHandle::get_library_store(){
  return _lib_store;
}



static const char* _symbol_not_found_format = "Cannot find function {0} in Lua Debugging API Library.";

#if (_WIN64) || (_WIN32)
LibLuaStore::LibLuaStore(HMODULE library_handle){
  _lib_handle = library_handle;
  _function_data = new function_data();

#define __CHECK_MODULE_FUNCTION(fd_address, fd_type, fname) \
  fd_address = (fd_type)GetProcAddress(library_handle, fname); \
  if(!fd_address){ \
    String _err_str = gd_format_str(_symbol_not_found_format, String(fname)); \
    GameUtils::Logger::print_err_static(_err_str); \
    if(!_error_message) \
      _error_message = new std::string(); \
     \
    *_error_message = GDSTR_TO_STDSTR(_err_str); \
    _error_code = GetLastError(); \
  }

  __CHECK_MODULE_FUNCTION(_function_data->get_cc, get_api_compilation_context, CPPLUA_GET_API_COMPILATION_CONTEXT_STR)
  __CHECK_MODULE_FUNCTION(_function_data->create_io_handler, create_library_io_handler_func, CPPLUA_LIBRARY_CREATE_IO_HANDLER_STR)
  __CHECK_MODULE_FUNCTION(_function_data->delete_io_handler, delete_library_io_handler_func, CPPLUA_LIBRARY_DELETE_IO_HANDLER_STR)
  __CHECK_MODULE_FUNCTION(_function_data->create_file_handler, create_library_file_handler_func, CPPLUA_LIBRARY_CREATE_FILE_HANDLER_STR)
  __CHECK_MODULE_FUNCTION(_function_data->delete_file_handler, delete_library_file_handler_func, CPPLUA_LIBRARY_DELETE_FILE_HANDLER_STR)
}
#elif (__linux)
LibLuaStore::LibLuaStore(void* library_handle){
  _lib_handle = library_handle;
  _function_data = new function_data();

#define __CHECK_MODULE_FUNCTION(fd_address, fd_type, fname) \
  fd_address = (fd_type)dlsym(library_handle, fname); \
  if(!fd_address){ \
    String _err_str = gd_format_str(_symbol_not_found_format, String(fname)); \
    GameUtils::Logger::print_er_static(_err_str); \
    if(!_error message) \
      _error_message = new std::string(); \
     \
    *_error_message = GDSTR_TO_STDSTR(_err_str); \
    _error_code = ERR_CANT_RESOLVE; \
  }

  __CHECK_MODULE_FUNCTION(_function_data->get_cc, get_api_compilation_context, CPPLUA_GET_API_COMPILATION_CONTEXT_STR)
  __CHECK_MODULE_FUNCTION(_function_data->create_io_handler, create_library_io_handler_func, CPPLUA_LIBRARY_CREATE_IO_HANDLER_STR)
  __CHECK_MODULE_FUNCTION(_function_data->delete_io_handler, delete_library_io_handler_func, CPPLUA_LIBRARY_DELETE_IO_HANDLER_STR)
  __CHECK_MODULE_FUNCTION(_function_data->create_file_handler, create_library_file_handler_func, CPPLUA_LIBRARY_CREATE_FILE_HANDLER_STR)
  __CHECK_MODULE_FUNCTION(_function_data->delete_file_handler, delete_library_file_handler_func, CPPLUA_LIBRARY_DELETE_FILE_HANDLER_STR)
}
#endif

LibLuaStore::~LibLuaStore(){
#if (_WIN64) || (_WIN32)
  if(_lib_handle)
    FreeLibrary(_lib_handle);
#elif (__linux)
  if(_lib_handle)
    dlclose(_lib_handle);
#endif

  if(_error_message)
    delete _error_message;

  if(_function_data)
    delete _function_data;
}


#if (_WIN64) || (_WIN32)
HMODULE LibLuaStore::get_library_handle(){
  return _lib_handle;
}
#elif (__linux)
void* LibLuaStore::get_library_handle(){
  return _lib_handle;
}
#endif


bool LibLuaStore::error_occurred() const{
  return _error_message;
}

int LibLuaStore::get_error_code() const{
  return _error_code;
}

std::string LibLuaStore::get_error_message() const{
  if(_error_message)
    return *_error_message;

  return "";
}


const LibLuaStore::function_data* LibLuaStore::get_function_data(){
  return _function_data;
}
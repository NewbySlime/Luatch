#ifndef LIBLUA_HANDLE_HEADER
#define LIBLUA_HANDLE_HEADER

#include "godot_cpp/classes/node.hpp"

#include "Lua-CPPAPI/Src/luadebug_variable_watcher.h"
#include "Lua-CPPAPI/Src/luaglobal_print_override.h"
#include "Lua-CPPAPI/Src/lualibrary_iohandler.h"
#include "Lua-CPPAPI/Src/luaruntime_handler.h"
#include "Lua-CPPAPI/Src/luavariant.h"

#include "memory"

#if (_WIN64) || (_WIN32)
#include "Windows.h"
#elif (__linux)
#include "dlfcn.h"
#endif


class LibLuaStore;
class LibLuaHandle: public godot::Node{
  GDCLASS(LibLuaHandle, godot::Node)

  private:
    std::shared_ptr<LibLuaStore> _lib_store;

    void _load_library();
    void _unload_library();

  protected:
    static void _bind_methods();

  public:
    LibLuaHandle();
    ~LibLuaHandle();

    void _ready() override;

    std::shared_ptr<LibLuaStore> get_library_store();
};


// Library handles passed to this object will be fully handled by this class. If the object is deleted, the handle also freed.
class LibLuaStore{
  public:
    struct function_data{
      public:
        // Get compilation_context of the Library
        get_api_compilation_context get_cc;

        create_library_io_handler_func create_io_handler;
        delete_library_io_handler_func delete_io_handler;

        create_library_file_handler_func create_file_handler;
        delete_library_file_handler_func delete_file_handler;
    };

  private:
#if (_WIN64) || (_WIN32)
    HMODULE _lib_handle = NULL;
#elif (__linux)
    void* _lib_handle = NULL;
#endif

    function_data* _function_data = NULL;

    int _error_code;
    std::string* _error_message = NULL;


  public:
#if (_WIN64) || (_WIN32)
    LibLuaStore(HMODULE library_handle);
#elif (__linux)
    LibLuaStore(void* library_handle);
#endif
  
    LibLuaStore(){}
    ~LibLuaStore();

#if (_WIN64) || (_WIN32)
    HMODULE get_library_handle();
#elif (__linux)
    void* get_library_handle();
#endif

    bool error_occurred() const;
    int get_error_code() const;
    std::string get_error_message() const;

    const LibLuaStore::function_data* get_function_data();
};


#endif
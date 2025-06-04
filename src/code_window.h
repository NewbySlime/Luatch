#ifndef CODE_WINDOW_HEADER
#define CODE_WINDOW_HEADER

#include "code_context.h"
#include "code_context_menu.h"
#include "console_window.h"
#include "luaprogram_handle.h"
#include "path_node.h"

#include "godot_cpp/classes/packed_scene.hpp"
#include "godot_cpp/classes/tab_container.hpp"
#include "godot_cpp/variant/node_path.hpp"


class CodeWindow: public godot::TabContainer{
  GDCLASS(CodeWindow, godot::TabContainer)

  public:
    // Param:
    //  - STRING: file_path
    static const char* s_file_loaded;
    // Param:
    //  - STRING: file_path
    static const char* s_file_closed;
    // Param:
    //  - STRING: file_path
    static const char* s_file_closing;
    static const char* s_focus_switched;
    // Param:
    //  - STRING: file_path
    //  - INT: line
    static const char* s_breakpoint_added;
    // Param:
    //  - STRING: file_path
    //  - INT: line
    static const char* s_breakpoint_removed;
    // Param:
    //  - STRING: file_path
    static const char* s_code_opened;
    // Param:
    //  - STRING: file_path
    //  - INT: error_code
    static const char* s_code_cannot_open;

    enum open_flag{
      open_flag_save_at_quit = 0b01,
      open_flag_allow_editing = 0b10
    };

  private:
    struct _node_data{
      CodeContext* _code_node = NULL;
    };

    struct _context_data{
      CodeContext* this_obj;
      int open_flag;
    };

    godot::String _code_context_scene_path;
    godot::Ref<godot::PackedScene> _code_context_scene;

    godot::NodePath _context_menu_path;
    CodeContextMenu* _context_menu_node;

    LuaProgramHandle* _program_handle;

    TPathNode<_node_data> _path_code_node;
    std::map<uint64_t, _context_data> _context_map;

    std::string _initial_prompt_path;

    bool _initialized = false;
    bool _context_menu_node_init = false;

    void _on_file_loaded(const godot::String& file_path, godot::Node* node);
    void _on_breakpoint_added(int line, uint64_t id);
    void _on_breakpoint_removed(int line, uint64_t id);
    void _on_file_cannot_open(const godot::String& file_path, int error_code);

    void _on_code_context_exiting(godot::Node* node);

    void _on_files_dropped(const godot::PackedStringArray& file_list);

    void _lua_on_started();
    void _lua_on_paused();
    void _lua_on_stopped();

    void _on_context_menu_button_pressed(int button_type);

    void _update_context_button_visibility();

    void _on_code_context_menu_ready(CodeContextMenu* obj);
    void _on_code_context_menu_ready_event(godot::Object* obj);

    void _on_thread_initialized();

    void _on_file_selected(const godot::String& str, godot::Node* node);
    void _on_files_selected(const godot::PackedStringArray& list, godot::Node* node);
  
  protected:
    static void _bind_methods();

  public:
    void _ready() override;

    void change_focus_code_context(const std::string& file_path);
    
    std::string get_current_focus_code() const;
    std::string get_current_focus_code_path() const;

    void open_code_context();
    // for the result of the function, listen to s_code_opened or s_code_cannot_open
    void open_code_context(const std::string& file_path, int flags = 0);

    bool close_current_code_context();
    bool close_code_context(const std::string& file_path);

    void run_current_code_context();

    CodeContext* get_current_code_context();
    CodeContext* get_code_context(const std::string& file_path);


    godot::String get_code_context_scene_path() const;
    void set_code_context_scene_path(godot::String scene_path);

    godot::NodePath get_context_menu_path() const;
    void set_context_menu_path(godot::NodePath path);
};

#endif
#ifndef UI_CONTROL_HEADER
#define UI_CONTROL_HEADER

#include "group_invoker.h"

#include "godot_cpp/classes/node.hpp"


class UIControl: public godot::Node{
  GDCLASS(UIControl, godot::Node)

  private:
    GroupInvoker* _ginvoker;

    godot::Vector2 _target_min_size = godot::Vector2(0, 0);
    godot::NodePath _target_min_size_node_path;

    godot::Callable _config_get_function;

    void _on_data_changed(const godot::String& key);
    void _on_data_changed_code_window_font();
    void _on_data_changed_inspector_font();
    void _on_data_changed_console_window_font();
    void _on_data_changed_ui_scale();

    void _on_target_min_size_changed(godot::Node* target_node);

    void _on_data_loaded();

    void _update_window_min_size();

    static void _code_init();

  protected:
    static void _bind_methods();

  public:
    void _ready() override;

    void set_target_min_size_node_path(const godot::NodePath& path);
    godot::NodePath get_target_min_size_node_path() const;
};

#endif
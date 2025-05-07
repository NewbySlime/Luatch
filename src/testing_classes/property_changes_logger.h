#ifndef PROPERTY_CHANGES_LOGGER_HEADER
#define PROPERTY_CHANGES_LOGGER_HEADER

#include "godot_cpp/classes/node.hpp"
#include "godot_cpp/variant/node_path.hpp"


class PropertyChangesLogger: public godot::Node{
  GDCLASS(PropertyChangesLogger, godot::Node)

  private:
    godot::NodePath _target_node_path;
    godot::Node* _target_node = NULL;

    godot::Array _watch_list;
    godot::Array _function_watch_list;
    godot::Dictionary _property_data;
    godot::Dictionary _function_data;

    void _on_property_changed();
    void _update_function_changes();


  protected:
    static void _bind_methods();

  public:
    void _ready() override;
    void _process(double delta) override;

    godot::NodePath get_target_node_path() const;
    void set_target_node_path(const godot::NodePath& path);

    godot::Array get_property_watch_list() const;
    void set_property_watch_list(const godot::Array& watch_list);
    
    godot::Array get_function_watch_list() const;
    void set_function_watch_list(const godot::Array& watch_list);
};

#endif
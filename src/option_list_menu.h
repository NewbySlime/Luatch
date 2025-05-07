#ifndef OPTION_LIST_MENU_HEADER
#define OPTION_LIST_MENU_HEADER

#include "godot_cpp/classes/control.hpp"

#include "map"


// TODO
//  [v] This class should not be as a parent, this should be as a child of a target parent, as that parent might need another type of control class.
//  [v] All classes that use OptionListMenu might need to use find_node function to get this class.
//  [v] Use property for if using the target list of itself or actually a target node.
//  [ ] Refactor this class to use godot::Node
class OptionValueControl;
class OptionListMenu: public godot::Control{
  GDCLASS(OptionListMenu, godot::Control)

  public:
    // Param:
    //  - STRING: key
    //  - ANY: value
    static const char* s_value_set;

  private:
    std::map<godot::String, OptionValueControl*> _option_lists;

    godot::NodePath _target_list_node_path;
    godot::Node* _target_list_node = NULL;

    void _on_option_changed(const godot::String& key, const godot::Variant& value);

    void _update_option_nodes(godot::Node* parent);

  protected:
    static void _bind_methods();

  public:
    void _ready() override;

    godot::NodePath get_target_list_node() const;
    void set_target_list_node(const godot::NodePath& path);

    // If relative_node is NULL, returned path is absolute.
    godot::NodePath get_value_control_path(const godot::String& key, godot::Node* relative_node = NULL) const;
    OptionValueControl* get_value_control_node(const godot::String& key) const;

    void set_value_data(const godot::String& key, const godot::Variant& value);
    godot::Variant get_value_data(const godot::String& key) const;

    godot::Array get_option_keys() const;
};

#endif
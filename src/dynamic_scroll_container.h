#ifndef DYNAMIC_SCROLL_CONTAINER_HEADER
#define DYNAMIC_SCROLL_CONTAINER_HEADER

#include "godot_cpp/classes/control.hpp"
#include "godot_cpp/classes/scroll_container.hpp"


// This class will try to resize itself to fit child's size. But its constraints are from the parent.
// This class does not work if the resizing work is done by parent.
class DynamicScrollContainer: public godot::ScrollContainer{
  GDCLASS(DynamicScrollContainer, godot::ScrollContainer)

  public:
    // Param:
    //  - size VECTOR2: The unformatted size that is going to be used.
    //  - cb CALLBACK: Callback object to inform the appropriate size.
    //    - new_size VECTOR2: The new size for the resizing action.
    static const char* s_about_to_resize;

  private:
    godot::Control* _current_child = NULL;
    godot::Control* _current_parent = NULL;

    godot::Vector2 _new_size;

    // to prevent infinite recursing
    bool _currently_signalled = false;

    bool _use_maximum_size = false;
    godot::Vector2 _maximum_size;

    void _bind_child(godot::Control* node);
    void _unbind_child(godot::Control* node);

    void _bind_parent(godot::Control* node);
    void _unbind_parent(godot::Control* node);

    void _check_child_target();
    void _check_parent_target();

    void _cb_change_new_size(const godot::Vector2& size);

    void _on_child_resized();
    void _on_parent_resized();

    void _on_child_entered_node(godot::Node* node);
    void _on_child_exiting_node(godot::Node* node);

  protected:
    static void _bind_methods();

  public:
    void _ready() override;
    void _notification(int code);

    bool get_use_maximum_size() const;
    void set_use_maximum_size(bool flag);

    godot::Vector2 get_maximum_size() const;
    void set_maximum_size(const godot::Vector2& size);
};

#endif
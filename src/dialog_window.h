// [v] TODO
//  [v] This class is basically ConfirmationDialog but with extendable options.
//  [v] Add property:
//    [v] dialog_text
//    [v] button_list
//    [v] hide_on_button_clicked
//  [v] Add function:
//    [v] Set button list with data like callbacks and such
//  [v] Add signal:
//    [v] choice_pressed
//  [v] Add pivot node as a basis of the resizing.
//    [v] This also keeps updated by checking its hierarchy. Always use the first child.


#ifndef DIALOG_WINDOW_HEADER
#define DIALOG_WINDOW_HEADER

#include "godot_cpp/classes/rich_text_label.hpp"
#include "godot_cpp/classes/window.hpp"

#include "map"
#include "vector"


class DialogWindow: public godot::Window{
  GDCLASS(DialogWindow, godot::Window)

  public:
    // Signal for when the button is pressed.
    // Param:
    //    button_key STRING: the choice button key that is pressed.
    static const char* s_on_button_pressed;

    struct ChoiceData{
      godot::String key;
      godot::String readable_name;
      bool hide_on_choosen = true;
      godot::Callable pressed_callback;
    };

  private:
    std::vector<ChoiceData*> _choice_list;
    std::map<godot::String, ChoiceData*> _choice_map; 

    godot::String _text_data;

    godot::RichTextLabel* _text_node = NULL;
    godot::Control* _button_pivot_node = NULL;

    godot::Control* _ui_pivot_node = NULL;

    void _update_size();
    void _update_pivot_node();

    void _on_pivot_node_resized();
    void _on_child_order_changed();
    void _on_button_pressed(const godot::String& key);
    void _on_popup();

    void _bind_pivot_node(godot::Control* node);
    void _unbind_pivot_node(godot::Control* node);

  protected:
    static void _bind_methods();

  public:
    ~DialogWindow();
  
    void _ready() override;

    void set_text(const godot::String& text);
    godot::String get_text() const;

    void add_new_choice(const ChoiceData& data);
    void clear_choice_list();
    
    size_t get_choice_data_count() const;
    const ChoiceData* get_choice_data(size_t idx) const;
    const ChoiceData* get_choice_data(const godot::String& key) const;
};

#endif
#ifndef ERROR_TRIGGER_INITIALIZER_HEADER
#define ERROR_TRIGGER_INITIALIZER_HEADER

#include "dialog_window.h"

#include "godot_cpp/classes/node.hpp"


class ErrorTriggerInitializer: public godot::Node{
  GDCLASS(ErrorTriggerInitializer, godot::Node)

  private:
    void _trigger_error_safe(DialogWindow* window);

    void _on_error_trigger_pressed(const godot::String& key, godot::Node* trigger_node);

  protected:
    static void _bind_methods();

  public:
    void _ready() override;

    void trigger_error(const char* message);
};

#endif
#ifndef WINDOW_UPDATER_HEADER
#define WINDOW_UPDATER_HEADER

#include "godot_cpp/classes/node.hpp"
#include "godot_cpp/classes/window.hpp"


class WindowUpdater: public godot::Node{
  GDCLASS(WindowUpdater, godot::Node)

  private:
    godot::Callable _data_set_function;
    godot::Callable _data_get_function;

    godot::Vector2i _last_window_position;
    int _last_window_mode;

    void _on_data_loaded();
    void _on_window_size_changed(godot::Window* window);

  protected:
    static void _bind_methods();

  public:
    void _ready() override;
    void _process(double delta) override;
};

#endif
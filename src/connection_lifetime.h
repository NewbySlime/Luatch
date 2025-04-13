#ifndef CONNECTION_LIFETIME_HEADER
#define CONNECTION_LIFETIME_HEADER

#include "godot_cpp/variant/variant.hpp"


class ConnectionLifetime{
  private:
    godot::Signal _current_signal;
    godot::Callable _current_callable;

    bool _invalid = false;

  public:
    ConnectionLifetime(const godot::Signal& signal, const godot::Callable& callable);
    ~ConnectionLifetime();

    void set_invalid(bool flag);

    godot::Signal get_current_signal();
    godot::Callable get_current_callable();
};

#endif
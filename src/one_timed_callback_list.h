#ifndef ONE_TIMED_CALLABACK_LIST_HEADER
#define ONE_TIMED_CALLABACK_LIST_HEADER

#include "godot_cpp/variant/callable.hpp"

#include "vector"


class OneTimedCallbackList{
  private:
    std::vector<godot::Callable> _callable_list;

  public:
    void append(const godot::Callable& callable);
    void clear_list();

    void update();
};

#endif
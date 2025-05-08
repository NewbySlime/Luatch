#include "one_timed_callback_list.h"

#include "godot_cpp/variant/variant.hpp"


using namespace godot;


void OneTimedCallbackList::append(const godot::Callable& cb){
  _callable_list.insert(_callable_list.end(), cb);
}

void OneTimedCallbackList::clear_list(){
  _callable_list.clear();
}


void OneTimedCallbackList::update(){
  for(Callable cb: _callable_list)
    cb.call();

  clear_list();
}
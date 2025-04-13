#include "connection_lifetime.h"


using namespace godot;


ConnectionLifetime::ConnectionLifetime(const Signal& signal, const Callable& callable){
  _current_signal = signal;
  _current_callable = callable;
}

ConnectionLifetime::~ConnectionLifetime(){
  if(_invalid)
    return;

  _current_signal.disconnect(_current_callable);
}


void ConnectionLifetime::set_invalid(bool flag){
  _invalid = flag;
}


Signal ConnectionLifetime::get_current_signal(){
  return _current_signal;
}

Callable ConnectionLifetime::get_current_callable(){
  return _current_callable;
}
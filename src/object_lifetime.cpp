#include "object_lifetime.h"


ObjectLifetime::ObjectLifetime(void* object, destructor_function destructor_fn){
  _obj = std::shared_ptr<void>(object, destructor_fn);
}

ObjectLifetime::ObjectLifetime(const std::shared_ptr<void>& obj){
  _obj = obj;
}

ObjectLifetime::ObjectLifetime(const ObjectLifetime& obj){
  _obj = obj._obj;
}


bool ObjectLifetime::is_valid_object() const{
  return _obj != NULL;
}
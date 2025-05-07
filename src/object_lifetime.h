#ifndef OBJECT_LIFETIME_HEADER
#define OBJECT_LIFETIME_HEADER

#include "memory"

// A wrapper object to std::shared_ptr object for clarity purposes.


class ObjectLifetime{
  public:
    typedef void(*destructor_function)(void* obj);

  protected:
    std::shared_ptr<void> _obj;

  public:
    ObjectLifetime(){}
    ObjectLifetime(void* object, destructor_function destructor_fn);
    ObjectLifetime(const std::shared_ptr<void>& obj);
    ObjectLifetime(const ObjectLifetime& obj);

    bool is_valid_object() const;
    
    template<typename T_Object> ObjectLifetime(T_Object* object, void(*deleter_function)(T_Object* obj) = NULL){
      if(!deleter_function){
        deleter_function = [](T_Object* obj){
          delete obj;
        }
      }

      _obj = std::shared_ptr<T_Object>(obj, deleter_function);
    }
};

#endif
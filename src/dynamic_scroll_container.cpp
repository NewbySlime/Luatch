#include "dynamic_scroll_container.h"


using namespace godot;


const char* DynamicScrollContainer::s_about_to_resize = "about_to_resize";


void DynamicScrollContainer::_bind_methods(){
  ClassDB::bind_method(D_METHOD("_on_child_entered_node", "node"), &DynamicScrollContainer::_on_child_entered_node);
  ClassDB::bind_method(D_METHOD("_on_child_exiting_node", "node"), &DynamicScrollContainer::_on_child_exiting_node);
  ClassDB::bind_method(D_METHOD("_cb_change_new_size", "size"), &DynamicScrollContainer::_cb_change_new_size);
  ClassDB::bind_method(D_METHOD("_on_child_resized"), &DynamicScrollContainer::_on_child_resized);
  ClassDB::bind_method(D_METHOD("_on_parent_resized"), &DynamicScrollContainer::_on_parent_resized);

  ClassDB::bind_method(D_METHOD("get_use_maximum_size"), &DynamicScrollContainer::get_use_maximum_size);
  ClassDB::bind_method(D_METHOD("set_use_maximum_size", "size"), &DynamicScrollContainer::set_use_maximum_size);

  ClassDB::bind_method(D_METHOD("get_maximum_size"), &DynamicScrollContainer::get_maximum_size);
  ClassDB::bind_method(D_METHOD("set_maximum_size", "size"), &DynamicScrollContainer::set_maximum_size);

  ADD_PROPERTY(PropertyInfo(Variant::BOOL, "use_maximum_size"), "set_use_maximum_size", "get_use_maximum_size");
  ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "maximum_size"), "set_maximum_size", "get_maximum_size");

  ADD_SIGNAL(MethodInfo(DynamicScrollContainer::s_about_to_resize, PropertyInfo(Variant::VECTOR2, "new_size"), PropertyInfo(Variant::CALLABLE, "cb")));
}


void DynamicScrollContainer::_bind_child(Control* node){
  node->connect("resized", Callable(this, "_on_child_resized"));
}

void DynamicScrollContainer::_unbind_child(Control* node){
  node->disconnect("resized", Callable(this, "_on_child_resized"));
}


void DynamicScrollContainer::_bind_parent(Control* node){
  node->connect("resized", Callable(this, "_on_parent_resized"));
}

void DynamicScrollContainer::_unbind_parent(Control* node){
  node->disconnect("resized", Callable(this, "_on_parent_resized"));
}


void DynamicScrollContainer::_check_child_target(){
  Control* _new_child = NULL;
  for(size_t i = 0; i < get_child_count(); i++){
    Node* _check_node = get_child(i);
    if(!_check_node->is_class(Control::get_class_static()))
      continue;
    
    _new_child = (Control*)_check_node;
    break;
  }

  if(_current_child == _new_child)
    return;

  if(_current_child)
    _unbind_child(_current_child);

  if(_new_child)
    _bind_child(_new_child);

  _current_child = _new_child;
  _on_child_resized();
}

void DynamicScrollContainer::_check_parent_target(){
  Control* _new_parent = NULL;
  Node* _check_node = get_parent();
  if(_check_node->is_class(Control::get_class_static()))
    _new_parent = (Control*)_check_node;
    
  if(_current_parent == _new_parent)
    return;

  if(_current_parent)
    _unbind_parent(_current_parent);

  if(_new_parent)
    _bind_parent(_new_parent);

  _current_parent = _new_parent;
  _on_parent_resized();
}


void DynamicScrollContainer::_cb_change_new_size(const Vector2& size){
  _new_size = size;
}


void DynamicScrollContainer::_on_child_resized(){
  if(_currently_signalled)
    return;
  
  if(!_current_child)
    return;

  _currently_signalled = true;

  _new_size = _current_child->get_size();
  emit_signal(s_about_to_resize, _new_size, Callable(this, "_cb_change_new_size"));
  
  if(_current_parent){
    Vector2 _clamp_size = _current_parent->get_size();
    _new_size = _new_size.clamp(Vector2(), _clamp_size);
  }

  if(_use_maximum_size)
    _new_size = _new_size.clamp(Vector2(), _maximum_size);

  set_size(_new_size);

  _currently_signalled = false;
}

void DynamicScrollContainer::_on_parent_resized(){
  if(!_current_parent)
    return;

  _on_child_resized();
}


void DynamicScrollContainer::_on_child_entered_node(Node* node){
  _check_child_target();
}

void DynamicScrollContainer::_on_child_exiting_node(Node* node){
  _check_child_target();
}


void DynamicScrollContainer::_ready(){
  connect("child_entered_tree", Callable(this, "_on_child_entered_node"));
  connect("child_exiting_tree", Callable(this, "_on_child_exiting_node"));

  _check_child_target();
}

void DynamicScrollContainer::_notification(int code){
  if(code == NOTIFICATION_PARENTED || code == NOTIFICATION_UNPARENTED)
    _check_parent_target();
}



bool DynamicScrollContainer::get_use_maximum_size() const{
  return _use_maximum_size;
}

void DynamicScrollContainer::set_use_maximum_size(bool flag){
  _use_maximum_size = flag;
  _on_child_resized();
}


Vector2 DynamicScrollContainer::get_maximum_size() const{
  return _maximum_size;
}

void DynamicScrollContainer::set_maximum_size(const Vector2& size){
  _maximum_size = size;
  _on_child_resized();
}
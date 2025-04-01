#include "data_node.h"


using namespace godot;


void DataNode::_bind_methods(){
  ClassDB::bind_method(D_METHOD("set_data", "key", "value"), &DataNode::set_data);
  ClassDB::bind_method(D_METHOD("get_data", "key"), &DataNode::get_data);
}


DataNode::~DataNode(){
  if(_data_dict)
    delete _data_dict;
}


void DataNode::_set_data(const Variant& key, const Variant& value){
  if(!_data_dict)
    _data_dict = new Dictionary();

  _data_dict->operator[](key) = value;
}

Variant DataNode::_get_data(const Variant& key){
  if(!_data_dict)
    return Variant();

  return _data_dict->operator[](key);
}


void DataNode::set_data(const Variant& key, const Variant& value){
  this->_set_data(key, value);
}

Variant DataNode::get_data(const Variant& key){
  return this->_get_data(key);
}
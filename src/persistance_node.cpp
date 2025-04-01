#include "persistance_node.h"


using namespace godot;


const char* PersistanceNode::s_data_loaded = "data_loaded";
const char* PersistanceNode::s_data_saving = "data_saving";


void PersistanceNode::_bind_methods(){
  ClassDB::bind_method(D_METHOD("save_data"), &PersistanceNode::save_data);
  ClassDB::bind_method(D_METHOD("load_data"), &PersistanceNode::load_data);
  ClassDB::bind_method(D_METHOD("is_data_loaded"), &PersistanceNode::is_data_loaded);

  ADD_SIGNAL(MethodInfo(PersistanceNode::s_data_loaded));
  ADD_SIGNAL(MethodInfo(PersistanceNode::s_data_saving));
}


void PersistanceNode::save_data(){
  emit_signal(PersistanceNode::s_data_saving);
  this->_save_data();
}

void PersistanceNode::load_data(){
  this->_load_data();
  emit_signal(PersistanceNode::s_data_loaded);
}


bool PersistanceNode::is_data_loaded(){
  return this->_is_data_loaded();
}
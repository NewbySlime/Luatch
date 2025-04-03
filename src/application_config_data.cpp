#include "application_config_data.h"

#include "godot_cpp/classes/engine.hpp"


using namespace godot;


const char* ApplicationConfigData::s_data_changed = "data_changed";
const char* ApplicationConfigData::default_option_config_file_path = "config.json";


void ApplicationConfigData::_bind_methods(){
  ClassDB::bind_method(D_METHOD("set_config_file_path", "path"), &ApplicationConfigData::set_config_file_path);
  ClassDB::bind_method(D_METHOD("get_config_file_path"), &ApplicationConfigData::get_config_file_path);

  ClassDB::bind_method(D_METHOD("set_default_data", "data"), &ApplicationConfigData::set_default_data);
  ClassDB::bind_method(D_METHOD("get_default_data"), &ApplicationConfigData::get_default_data);

  ADD_PROPERTY(PropertyInfo(Variant::STRING, "config_file_path", PropertyHint::PROPERTY_HINT_SAVE_FILE), "set_config_file_path", "get_config_file_path");
  ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "default_data"), "set_default_data", "get_default_data");
  
  ADD_SIGNAL(MethodInfo(ApplicationConfigData::s_data_changed, PropertyInfo(Variant::STRING, "key")));
}


ApplicationConfigData::~ApplicationConfigData(){
  if(_file_handle)
    delete _file_handle;
}


void ApplicationConfigData::_set_data(const Variant& key, const Variant& value){
  _file_handle->set_value(key, value);
  _file_handle->save_data();

  emit_signal(ApplicationConfigData::s_data_changed, key);
}

Variant ApplicationConfigData::_get_data(const Variant& key){
  return _file_handle->get_value(key);
}


void ApplicationConfigData::_save_data(){
  _file_handle->save_data();
}

void ApplicationConfigData::_load_data(){
  _file_handle->refresh_data();
  if(_check_missing_data(_file_handle->get_data(), _default_data))
    _file_handle->save_data();
    
  _is_loaded = true;
}


bool ApplicationConfigData::_is_data_loaded(){
  return _is_loaded;
}


bool ApplicationConfigData::_check_missing_data(Dictionary& target, const Dictionary& comparison){
  bool _is_missing = false;

  Array _key_list = comparison.keys();
  for(int i = 0; i < _key_list.size(); i++){
    Variant _key = _key_list[i];
    if(target.has(_key))
      continue;

    target[_key] = comparison[_key];
    _is_missing = true;
  }

  return _is_missing;
}


void ApplicationConfigData::_ready(){
  Engine* _engine = Engine::get_singleton();
  if(_engine->is_editor_hint())
    return;

  _file_handle = new JsonFileHandle(_config_file_path);
  load_data();
}


void ApplicationConfigData::set_config_file_path(const String& path){
  _config_file_path = path;
}

String ApplicationConfigData::get_config_file_path() const{
  return _config_file_path;
}


void ApplicationConfigData::set_default_data(const Dictionary& data){
  _default_data = data;
}

Dictionary ApplicationConfigData::get_default_data() const{
  return _default_data;
}
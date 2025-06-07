#include "application_metadata.h"
#include "defines.h"

#include "godot_cpp/classes/engine.hpp"
#include "godot_cpp/classes/project_settings.hpp"


using namespace godot;


void ApplicationMetadata::_bind_methods(){

}


ApplicationMetadata::~ApplicationMetadata(){
  if(_reg_handle){
    _reg_handle->save_data();
    delete _reg_handle;
  }
}


void ApplicationMetadata::_set_data(const Variant& key, const Variant& value){
  _reg_handle->set_value(key, value);
}

Variant ApplicationMetadata::_get_data(const Variant& key){
  return _reg_handle->get_value(key);
}


void ApplicationMetadata::_save_data(){
  _reg_handle->save_data();
}

void ApplicationMetadata::_load_data(){
  _reg_handle->refresh_data();
  _is_loaded = true;
}


bool ApplicationMetadata::_is_data_loaded(){
  return _is_loaded;
}


void ApplicationMetadata::_ready(){
  Engine* _engine = Engine::get_singleton();
  if(_engine->is_editor_hint())
    return;

  ProjectSettings* _project_config = ProjectSettings::get_singleton();
  String _project_name = _project_config->get("application/config/name");

#if (_WIN64) || (_WIN32)
  _reg_handle = new RegistryFileHandle("SOFTWARE\\" + GDSTR_TO_STDSTR(_project_name));
#elif (__linux)
  _reg_handle = new JsonFileHandle(".metadata");
#endif
  load_data();
}
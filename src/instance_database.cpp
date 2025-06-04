#include "instance_database.h"
#include "logger.h"
#include "strutil.h"

#include "godot_cpp/classes/engine.hpp"


using namespace godot;


void InstanceDatabase::_bind_methods(){

}


Dictionary InstanceDatabase::get_database_data() const{
  return _database_data;
}

void InstanceDatabase::set_database_data(const Dictionary& data){
  _database_data = data;

  Array _key_list = _database_data.keys();
  for(size_t i = 0; i < _key_list.size(); i++){
    godot::Ref<godot::PackedScene> _value = _database_data[_key_list[i]];
    if(!_value.is_null())
      continue;

    GameUtils::Logger::print_err_static(gd_format_str("[InstanceDatabase] '{0}' is not a valid PackedScene.", _key_list[i]));
  }
}


Ref<PackedScene> InstanceDatabase::get_instance(const String& class_name){
  return _database_data[class_name];
}
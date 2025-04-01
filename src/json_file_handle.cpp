#include "algorithm.h"
#include "defines.h"
#include "json_file_handle.h"
#include "logger.h"
#include "strutil.h"

#include "godot_cpp/classes/file_access.hpp"

#define KEY_SPECIAL_DATA_FLAG "__special_data_flag"
#define KEY_DATA "data"

using namespace godot;



JsonFileHandle::JsonFileHandle(const String& file_path){
  _file_path = file_path;
  refresh_data();
}


Dictionary& JsonFileHandle::get_data(){
  return _data;
}

const Dictionary& JsonFileHandle::get_data() const{
  return _data;
}


Variant JsonFileHandle::get_value(const Variant& key){
  return _data[key];
}

void JsonFileHandle::set_value(const Variant& key, const Variant& value){
  _data[key] = value;
}


void JsonFileHandle::refresh_data(){
  _data.clear();

  // skip refresh if not exists, let the JSON object filled with data
  if(!FileAccess::file_exists(_file_path))
    return;

  Ref<FileAccess> _file = FileAccess::open(_file_path, FileAccess::READ);
  if(_file.is_null()){
    GameUtils::Logger::print_err_static(gd_format_str("[JsonFileHandle] Cannot open file '{0}'. (Err Code {1})", _file_path, FileAccess::get_open_error()));
    return;
  }

{ // enclosure for using gotos
  std::shared_ptr<JSON> _json = std::shared_ptr<JSON>(memnew(JSON), memdelete<JSON>);
  Error _err = _json->parse(_file->get_as_text());
  if(_err != OK){
    GameUtils::Logger::print_err_static(gd_format_str("[JsonFileHandle] Cannot parse JSON file '{0}'. Reason: {1}", _file_path, _json->get_error_message()));
    goto skip_to_return;
  }

  _data = _json->get_data();
} // enclosure closing
  
  skip_to_return:{}
  _file->close();
}

void JsonFileHandle::save_data(){
  Ref<FileAccess> _file = FileAccess::open(_file_path, FileAccess::WRITE);
  if(_file.is_null()){
    GameUtils::Logger::print_err_static(gd_format_str("[JsonFileHandle] Cannot save JSON data. Reason: Cannot create '{0}' file. (Err code {1})", _file_path, FileAccess::get_open_error()));
    return;
  }

  _file->store_string(JSON::stringify(_data));
  _file->close();
}
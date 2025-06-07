#include "algorithm.h"
#include "defines.h"
#include "directory_util.h"
#include "json_file_handle.h"
#include "logger.h"
#include "strutil.h"

#include "godot_cpp/classes/file_access.hpp"

#include "memory"


#define KEY_DATA "___data___"


using namespace DirectoryUtil;
using namespace godot;



JsonFileHandle::JsonFileHandle(const String& file_path){
  _file_path = file_path;
  refresh_data();
}

void JsonFileHandle::_get_dict_data(std::vector<std::string>& split_data, int current_idx, std::function<void(godot::Variant&)> cb, Dictionary* current_dict){
  if(!current_dict)
    current_dict = &_data;

  String _next_key = split_data[current_idx].c_str();
  Dictionary _next_dict = current_dict->operator[](_next_key);
  if(current_idx != (split_data.size()-1))
    _get_dict_data(split_data, current_idx+1, cb, &_next_dict);
  else{
    Variant _value_data = _next_dict[KEY_DATA];
    cb(_value_data);
    _next_dict[KEY_DATA] = _value_data;
  }

  current_dict->operator[](_next_key) = _next_dict;
}


Dictionary& JsonFileHandle::get_data(){
  return _data;
}

const Dictionary& JsonFileHandle::get_data() const{
  return _data;
}


Variant JsonFileHandle::get_value(const Variant& key){
  std::vector<std::string> _str_array;
  String _key_str = key.stringify();

  Variant _result;
  split_directory_string(GDSTR_TO_STDSTR(_key_str), _str_array);
  _get_dict_data(_str_array, 0, [&](Variant& data){
    _result = data;
  });

  return _result;
}

void JsonFileHandle::set_value(const Variant& key, const Variant& value){
  std::vector<std::string> _str_array;
  String _key_str = key.stringify();

  split_directory_string(GDSTR_TO_STDSTR(_key_str), _str_array);
  _get_dict_data(_str_array, 0, [&](Variant& data){
    data = value;
  });
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
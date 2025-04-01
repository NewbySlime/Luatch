#if (_WIN64) || (_WIN32)

#include "defines.h"
#include "error_util.h"
#include "logger.h"
#include "registry_file_handle.h"
#include "strutil.h"
#include "Windows.h"


// including null-terminated string
#define VALUE_NAME_MAX_SIZE (16383+1)


using namespace error::util;
using namespace godot;


char _value_name_buffer[VALUE_NAME_MAX_SIZE];


RegistryFileHandle::RegistryFileHandle(const std::string& key_path){
  _key_path = key_path;
  refresh_data();
}


Dictionary RegistryFileHandle::_parse_data_from_hkey(HKEY key){
  Dictionary _data_dict;

  // increment the value
  for(int i = 0; ; i++){
    DWORD _name_len = VALUE_NAME_MAX_SIZE;
    DWORD _value_type = 0;
    DWORD _data_len = 0;
    LSTATUS _success = RegEnumValue(key, i, _value_name_buffer, &_name_len, 0, &_value_type, NULL, &_data_len);
    if(_success != ERROR_SUCCESS)
      break;

    _name_len = VALUE_NAME_MAX_SIZE;
    char* _data_buffer = (char*)malloc(_data_len);
    RegEnumValue(key, i, _value_name_buffer, &_name_len, 0, &_value_type, (LPBYTE)_data_buffer, &_data_len);

    Variant _value_data;
    switch(_value_type){
      break; case REG_BINARY:{
        PackedByteArray _data(std::initializer_list<uint8_t>((uint8_t*)_data_buffer, (uint8_t*)&_data_buffer[_data_len]));
        _value_data = UtilityFunctions::bytes_to_var(_data);
      }

      break; case REG_SZ:{
        // just to make sure
        _data_buffer[_data_len-1] = '\0';
        _value_data = UtilityFunctions::str_to_var(_data_buffer);
      }
    }

    free(_data_buffer);

    _data_dict[_value_name_buffer] = _value_data;
  }

  // increment the subkey
  for(int i = 0; ; i++){
    DWORD _name_len = VALUE_NAME_MAX_SIZE;
    LSTATUS _success = RegEnumKeyEx(key, i, _value_name_buffer, &_name_len, 0, NULL, NULL, NULL);
    if(_success != ERROR_SUCCESS)
      break;

    HKEY _subkey;
    _success = RegOpenKeyEx(key, _value_name_buffer, 0, KEY_READ, &_subkey);
    if(_success != ERROR_SUCCESS){
      GameUtils::Logger::print_err_static(gd_format_str("[RegistryFileHandle] Cannot open key '{0}'. Reason: {1}", _value_name_buffer, get_windows_error_message(_success).c_str()));
      continue;
    }

    // store it first, as it will be overriden when recursing
    String _value_name = _value_name_buffer;
    _data_dict[_value_name] = _parse_data_from_hkey(_subkey);
    RegCloseKey(_subkey);
  }

  return _data_dict;
}

void RegistryFileHandle::_store_data_to_hkey(HKEY key, const Dictionary& data){
  Array _key_list = data.keys();
  for(int i = 0; i < _key_list.size(); i++){
    Variant _key_data = _key_list[i];
    Variant _value_data = data[_key_data];
    String _key_data_gdstr = _key_data;
    std::string _key_data_str = GDSTR_TO_STDSTR(_key_data_gdstr);
    
    switch(_value_data.get_type()){
      break; case Variant::DICTIONARY:{
        HKEY _subkey;
        LSTATUS _success = RegOpenKeyEx(key, _key_data_str.c_str(), 0, KEY_WRITE, &_subkey);
        if(_success == ERROR_FILE_NOT_FOUND)
          _success = RegCreateKeyEx(key, _key_data_str.c_str(), 0, NULL, 0, KEY_WRITE, NULL, &_subkey, NULL);

        if(_success != ERROR_SUCCESS){
          GameUtils::Logger::print_err_static(gd_format_str("[RegistryFileHandle] Cannot open key '{0}'. Reason: {1}", _key_data_gdstr, get_windows_error_message(_success).c_str()));
          break;
        }

        _store_data_to_hkey(_subkey, _value_data);
        RegCloseKey(_subkey);
      }

      break; default:{
        PackedByteArray _bvalue = UtilityFunctions::var_to_bytes(_value_data);
    
        LSTATUS _success = RegSetValueEx(key, _key_data_str.c_str(), 0, REG_BINARY, _bvalue.ptr(), _bvalue.size());
        if(_success != ERROR_SUCCESS){
          GameUtils::Logger::print_err_static(gd_format_str("[RegistryFileHandle] Cannot set value of key '{0}'. Reason: {1}", _key_data_gdstr, get_windows_error_message(_success).c_str()));
          break;
        }
      }
    }
  }
}


Variant RegistryFileHandle::get_value(const Variant& key){
  return _data[key];
}

void RegistryFileHandle::set_value(const Variant& key, const Variant& value){
  _data[key] = value;
}


void RegistryFileHandle::refresh_data(){
  _data.clear();

  HKEY _hkey;
  LSTATUS _success = RegOpenKeyEx(HKEY_CURRENT_USER, _key_path.c_str(), 0, KEY_READ, &_hkey);
  if(_success != ERROR_SUCCESS){
    GameUtils::Logger::print_err_static(gd_format_str("[RegistryFileHandle] Cannot open '{0}' registry. Reason: {1}", _key_path.c_str(), get_windows_error_message(_success).c_str()));
    return;
  }

  _data = _parse_data_from_hkey(_hkey);
  RegCloseKey(_hkey);
}

void RegistryFileHandle::save_data(){
  HKEY _hkey;
  LSTATUS _success = RegOpenKeyEx(HKEY_CURRENT_USER, _key_path.c_str(), 0, KEY_WRITE, &_hkey);
  if(_success == ERROR_FILE_NOT_FOUND)
    _success = RegCreateKeyEx(HKEY_CURRENT_USER, _key_path.c_str(), 0, NULL, 0, KEY_WRITE, NULL, &_hkey, NULL);

  if(_success != ERROR_SUCCESS){
    GameUtils::Logger::print_err_static(gd_format_str("[RegistryFileHandle] Cannot open '{0}' registry. Reason: {1}", _key_path.c_str(), get_windows_error_message(_success).c_str()));
    return;
  }

  _store_data_to_hkey(_hkey, _data);
  RegCloseKey(_hkey);
}

#endif
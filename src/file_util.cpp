#include "debug_log.h"
#include "directory_util.h"
#include "error_util.h"
#include "file_util.h"
#include "stdio.h"
#include "strutil.h"
#include "time.h"

#if (_WIN64) || (_WIN32)
#include "Windows.h"
#endif


#define TEMPORARY_FILE_NAME_LEN 32

#if (_WIN64) || (_WIN32)
#define TEMPORARY_BASE_PATH "%localappdata%\\Temp\\"
#elif (__linux)
#define TEMPORARY_BASE_PATH "/tmp/"
#endif

#define TEMPORARY_COPY_FILE_ITERATION_MAX 3000

#define BUFFER_LEN 512


using namespace DirectoryUtil;
using namespace error::util;


std::string FileUtil::create_temporary_file_path(const std::string& additional_base_path, const std::string& extension){
  char* _name_buffer = NULL;

  std::string _result;
  
{ // enclosure for using gotos
  _name_buffer = (char*)malloc(TEMPORARY_FILE_NAME_LEN+1);
  for(int i = 0; i < TEMPORARY_FILE_NAME_LEN; i += 2){
    unsigned char _randint = rand();

    snprintf(&_name_buffer[i], TEMPORARY_FILE_NAME_LEN-i, "%X", _randint);
  }

  _name_buffer[TEMPORARY_FILE_NAME_LEN] = '\0';
  _result = get_temporary_base_folder() + additional_base_path + _name_buffer + extension;
} // enclosure closing

  skip_to_return:{}

  if(_name_buffer)
    free(_name_buffer);

  return _result;
}

std::string FileUtil::get_temporary_base_folder(){
  std::string _result;

#if (_WIN64) || (_WIN32)
  char* _result_buffer = NULL;
  DWORD _result_len = ExpandEnvironmentStringsA(TEMPORARY_BASE_PATH, NULL, 0);
  if(!_result_len){
    PRINT_LOG_ERR_VERBOSE(format_str(__FILE__":"__LINE__": %s", get_windows_error_message(GetLastError()).c_str()));
    return "";
  }
    
  _result_buffer = (char*)malloc(_result_len);
  _result_buffer[_result_len-1] = '\0';
  ExpandEnvironmentStringsA(TEMPORARY_BASE_PATH, _result_buffer, _result_len);
  _result = _result_buffer;

  free(_result_buffer);
#elif (__linux)
  _result = TEMPORARY_BASE_PATH;
#endif

  return _result;
}
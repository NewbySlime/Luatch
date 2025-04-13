#ifndef FILE_UTIL_HEADER
#define FILE_UTIL_HEADER

#include "string"


namespace FileUtil{
  std::string create_temporary_file_path(const std::string& additional_base_path, const std::string& extension);
  std::string get_temporary_base_folder();
}

#endif
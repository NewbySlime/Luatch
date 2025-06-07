#include "defines.h"
#include "directory_util.h"
#include "error_trigger.h"
#include "instance_database.h"
#include "logger.h"
#include "strutil.h"

#include "godot_cpp/classes/dir_access.hpp"
#include "godot_cpp/classes/engine.hpp"
#include "godot_cpp/classes/resource_loader.hpp"
#include "godot_cpp/classes/scene_tree.hpp"


using namespace DirectoryUtil;
using namespace ErrorTrigger;
using namespace godot;


void InstanceDatabase::_bind_methods(){
  ClassDB::bind_method(D_METHOD("get_database_folder"), &InstanceDatabase::get_database_folder);
  ClassDB::bind_method(D_METHOD("set_database_folder", "folder_path"), &InstanceDatabase::set_database_folder);

  ADD_PROPERTY(PropertyInfo(Variant::STRING, "database_folder", PROPERTY_HINT_DIR), "set_database_folder", "get_database_folder");
}


void InstanceDatabase::_ready(){
  Engine* _engine = Engine::get_singleton();
  if(_engine->is_editor_hint())
    return;

  int _quit_code;

{ // enclosure for using gotos
  Ref<DirAccess> _directory = DirAccess::open(_database_folder);
  if(_directory.is_null()){
    GameUtils::Logger::print_err_static("[InstanceDatabase] Folder data is not valid.");
    _quit_code = ERR_FILE_BAD_PATH;
    goto on_error;
  }

  ResourceLoader* _resloader = ResourceLoader::get_singleton();
  PackedStringArray _files_list = _directory->get_files();
  for(size_t i = 0; i < _files_list.size(); i++){
    String _file_name = _files_list[i];
    String _full_path = _database_folder + "/" + _file_name;
    
    Ref<Resource> _resource = _resloader->load(_full_path);
    if(_resource.is_null() || !_resource->is_class(PackedScene::get_class_static())){
      GameUtils::Logger::print_err_static(gd_format_str("[InstanceDatabase] '{0}' is not a valid Scene file.", _full_path));
      continue;
    }

    std::string _file_name_stripped = strip_filename_from_extension(GDSTR_TO_STDSTR(_file_name));
    _database_data[_file_name_stripped.c_str()] = _resource;
  }
} // enclosure closing
  return;


  on_error:{}
  trigger_generic_error_message();
  get_tree()->quit(_quit_code);
}


String InstanceDatabase::get_database_folder() const{
  return _database_folder;
}

void InstanceDatabase::set_database_folder(const String& folder_path){
  _database_folder = folder_path;
}


Ref<PackedScene> InstanceDatabase::get_instance(const String& class_name){
  return _database_data[class_name];
}
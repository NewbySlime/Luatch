#include "data_node.h"
#include "error_trigger.h"
#include "file_dialog_extend.h"
#include "logger.h"

#include "godot_cpp/classes/engine.hpp"
#include "godot_cpp/classes/os.hpp"
#include "godot_cpp/classes/scene_tree.hpp"


#define LAST_PATH_KEY "file_dialog_ui/last_folder_path"


using namespace ErrorTrigger;
using namespace godot;


void FileDialogExtend::_bind_methods(){
  ClassDB::bind_method(D_METHOD("_on_popup"), &FileDialogExtend::_on_popup);
  ClassDB::bind_method(D_METHOD("_on_confirmed"), &FileDialogExtend::_on_confirmed);
  ClassDB::bind_method(D_METHOD("_on_cancelled"), &FileDialogExtend::_on_cancelled);  
}


void FileDialogExtend::_update_last_path(){
  _application_metadata_set.call(LAST_PATH_KEY, get_current_dir());
}


void FileDialogExtend::_on_popup(){
  String _file_path = _set_folder_path;

  if(_set_folder_path.is_empty())
    _file_path = _application_metadata_get.call(LAST_PATH_KEY);

  if(_file_path.is_empty())
    _file_path = OS::get_singleton()->get_executable_path();

  _set_folder_path = "";

  _skip_set = false;
  set_current_dir(_file_path);
  _skip_set = true;
}


void FileDialogExtend::_on_confirmed(){
  _update_last_path();
}

void FileDialogExtend::_on_cancelled(){
  _update_last_path();
}


void FileDialogExtend::_ready(){
  Engine* _engine = Engine::get_singleton();
  if(_engine->is_editor_hint())
    return;

  int _quit_code;

{ // enclosure for using gotos
  DataNode* _data_node = get_node<DataNode>("/root/GlobalApplicationMetadata");
  if(!_data_node){
    GameUtils::Logger::print_err_static("[FileDialogExtend] Cannot get Application Metadata.");
    _quit_code = ERR_UNCONFIGURED;
    goto on_error;
  }

  _application_metadata_get = Callable(_data_node, "get_data");
  _application_metadata_set = Callable(_data_node, "set_data");

  connect("about_to_popup", Callable(this, "_on_popup"));
  connect("confirmed", Callable(this, "_on_confirmed"));
  connect("canceled", Callable(this, "_on_cancelled"));

  hide();
  set_force_native(true);
} // enclosure closing

  return;


  on_error:{}
  SceneTree* _tree = get_tree();
  trigger_generic_error_message([_tree, _quit_code](){
    _tree->quit(_quit_code);
  });
}

bool FileDialogExtend::_set(const StringName& key, const Variant& value){
  if(_skip_set)
    return false;

  if(key == (String)"current_dir"){
    _set_folder_path = value;
    return true;
  }

  return false;
}
// TODO
//  [v] Extend FileDialog class
//  [v] Each confirmation, store last folder path
//  [ ] Also store last position and size of the window


#ifndef FILE_DIALOG_EXTEND_HEADER
#define FILE_DIALOG_EXTEND_HEADER

#include "godot_cpp/classes/file_dialog.hpp"


class FileDialogExtend: public godot::FileDialog{
  GDCLASS(FileDialogExtend, godot::FileDialog)

  private:
    godot::Callable _application_metadata_set;
    godot::Callable _application_metadata_get;

    bool _skip_set = false;
    godot::String _set_folder_path;

    void _update_last_path();

    void _on_popup();

    void _on_confirmed();
    void _on_cancelled();    

  protected:
    static void _bind_methods();

  public:
    void _ready() override;

    bool _set(const godot::StringName& key, const godot::Variant& value);
};

#endif
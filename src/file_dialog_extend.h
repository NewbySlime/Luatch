// TODO
//  [ ] Extend FileDialog class
//  [ ] Each confirmation, store last folder path
//  [ ] Delete self when in windows


#ifndef FILE_DIALOG_EXTEND_HEADER
#define FILE_DIALOG_EXTEND_HEADER

#include "godot_cpp/classes/file_dialog.hpp"


class FileDialogExtend: public godot::FileDialog{
  GDCLASS(FileDialogExtend, godot::FileDialog)

  private:
    

  protected:
    static void _bind_methods();

  public:
    void _ready() override;


};

#endif
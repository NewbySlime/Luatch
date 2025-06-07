// TODO
//  [ ] Database of PackedScene for certain class
//  [ ] This should be stored inside a folder, each PackedScene name represents the key.

#ifndef INSTANCE_DATABASE_HEADER
#define INSTANCE_DATABASE_HEADER

#include "godot_cpp/classes/node.hpp"
#include "godot_cpp/classes/packed_scene.hpp"


class InstanceDatabase: public godot::Node{
  GDCLASS(InstanceDatabase, godot::Node)

  private:
    godot::String _database_folder;
    godot::Dictionary _database_data;

  protected:
    static void _bind_methods();

  public:
    void _ready() override;

    godot::String get_database_folder() const;
    void set_database_folder(const godot::String& folder_path);

    godot::Ref<godot::PackedScene> get_instance(const godot::String& class_name);

    template<typename T_class> godot::Ref<godot::PackedScene> get_instance(){
      return get_instance(T_class::get_class_static());
    }
};

#endif
// TODO
//  [ ] Database of PackedScene for certain class


#ifndef INSTANCE_DATABASE_HEADER
#define INSTANCE_DATABASE_HEADER

#include "godot_cpp/classes/node.hpp"
#include "godot_cpp/classes/packed_scene.hpp"


class InstanceDatabase: public godot::Node{
  GDCLASS(InstanceDatabase, godot::Node)

  private:
    godot::Dictionary _database_data;

  protected:
    static void _bind_methods();

  public:
    godot::Dictionary get_database_data() const;
    void set_database_data(const godot::Dictionary& data); 

    godot::Ref<godot::PackedScene> get_instance(const godot::String& class_name);

    template<typename T_class> godot::Ref<godot::PackedScene> get_instance(){
      return get_instance(T_class::get_class_static());
    }
};

#endif
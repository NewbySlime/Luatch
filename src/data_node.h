#ifndef DATA_NODE_HEADER
#define DATA_NODE_HEADER

#include "godot_cpp/classes/node.hpp"

class DataNode: public godot::Node{
  GDCLASS(DataNode, godot::Node)

  private:
    godot::Dictionary* _data_dict = NULL;

  protected:
    virtual void _set_data(const godot::Variant& key, const godot::Variant& value);
    virtual godot::Variant _get_data(const godot::Variant& key);

    static void _bind_methods();

  public:
    ~DataNode();

    void set_data(const godot::Variant& key, const godot::Variant& value);
    godot::Variant get_data(const godot::Variant& key);
};

#endif
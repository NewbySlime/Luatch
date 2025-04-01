#ifndef PERSISTANCE_NODE_HEADER
#define PERSISTANCE_NODE_HEADER

#include "data_node.h"


class PersistanceNode: public DataNode{
  GDCLASS(PersistanceNode, DataNode)

  public:
    static const char* s_data_loaded;
    static const char* s_data_saving;

  protected:
    virtual void _save_data(){}
    virtual void _load_data(){}

    virtual bool _is_data_loaded(){return false;}

    static void _bind_methods();

  public:
    void save_data();
    void load_data();

    bool is_data_loaded();
};

#endif
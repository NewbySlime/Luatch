#ifndef PATH_NODE_HEADER
#define PATH_NODE_HEADER

#include "directory_util.h"
#include "map"
#include "string"



class PathNode{
  public:
    typedef void* (*CopyConstructor)(void* data);
    typedef void (*DeleteConstructor)(void* data);

  private:
    struct _node_data{
      std::string name;
      std::map<std::string, _node_data*> branches;
      
      void* data = NULL;
  
      // If NULL, then it is the top most node.
      _node_data* parent = NULL;
    };

    CopyConstructor _copy_func;
    DeleteConstructor _delete_func;
  
    _node_data* _root = NULL;
  
    void _remove_node(_node_data* node);
    void _remove_node_branches(_node_data* node);
  
    _node_data* _get_node_data(const std::string& path) const;
  
  public:
    PathNode(CopyConstructor copy_func, DeleteConstructor delete_func);
    ~PathNode();
  
    // This function will create a copied data, based on the function supplied in PathNode constructor.
    // obj parameter can be NULL to create dummy node. To replace the object data, use set_node_data() function.
    // Returns the newly created object. If already existed, it will return the current object.
    void* create_node(const std::string& path, void* data = NULL);
    bool delete_node(const std::string& path);
    
    // Will return NULL when not found.
    void* get_node_data(const std::string& path) const;
    bool has_node(const std::string& path) const;
};


// T_object should be copy constructible.
template<typename T_object> class TPathNode{
  private:
    PathNode* _node;

  public:
    TPathNode(){
      _node = new PathNode(
        [](void* data){
          T_object* _obj = (T_object*)data;
          return (void*)new T_object(*_obj);
        },

        [](void* data){
          T_object* _obj = (T_object*)data;
          delete _obj;
        }
      );
    }

    T_object* create_node(const std::string& path, T_object* data = NULL){
      return (T_object*)_node->create_node(path, data);
    }

    bool delete_node(const std::string& path){
      return _node->delete_node(path);
    }


    T_object* get_node_data(const std::string& path) const{
      return (T_object*)_node->get_node_data(path); 
    }

    bool has_node(const std::string& path) const{
      return _node->has_node(path);
    }
};

#endif
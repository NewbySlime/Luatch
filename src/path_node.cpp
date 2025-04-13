#include "directory_util.h"
#include "path_node.h"


using namespace DirectoryUtil;


PathNode::PathNode(CopyConstructor copy_func, DeleteConstructor delete_func){
  _root = new _node_data();
  _copy_func = copy_func;
  _delete_func = delete_func;
}

PathNode::~PathNode(){
  if(!_root)
    return;

  _remove_node(_root);
}


void PathNode::_remove_node(_node_data* node){
  _remove_node_branches(node);
  _node_data* _iter_node = node;

  // deleting all nodes related to current node until a branch has another node
  while(_iter_node->parent){
    _node_data* _parent_node = _iter_node->parent;
    _parent_node->branches.erase(_iter_node->name);
    if(_iter_node->data)
      _delete_func(_iter_node->data);

    delete _iter_node;
    _iter_node = _parent_node;

    if(_parent_node->branches.size() > 0)
      break;
  }
}

void PathNode::_remove_node_branches(_node_data* node){
  for(auto _pair: node->branches){
    _remove_node_branches(_pair.second);
    if(_pair.second->data)
      _delete_func(_pair.second->data);

    delete _pair.second;
  }

  node->branches.clear();
}


PathNode::_node_data* PathNode::_get_node_data(const std::string& path) const{
  std::string _mod_path = DirectoryUtil::get_absolute_path(path);
  std::vector<std::string> _split_data; DirectoryUtil::split_directory_string(_mod_path, _split_data);

  _node_data* _node = _root;
  for(int i = 0; i < _split_data.size(); i++){
    if(!_node)
      break;

    auto _iter = _node->branches.find(_split_data[i]);
    if(_iter != _node->branches.end())
      _node = _iter->second;
    else
      _node = NULL;
  }

  return _node;
}


void* PathNode::create_node(const std::string& path, void* data){
  std::string _mod_path = DirectoryUtil::get_absolute_path(path);
  std::vector<std::string> _split_data; DirectoryUtil::split_directory_string(_mod_path, _split_data);

  _node_data* _node = _root;
  for(int i = 0; i < _split_data.size(); i++){
    auto _iter = _node->branches.find(_split_data[i]);
    if(_iter != _node->branches.end())
      _node = _iter->second;
    else{
      _node_data* _new_node = new _node_data();
      _new_node->name = _split_data[i];
      _new_node->parent = _node;

      _node->branches[_split_data[i]] = _new_node;
      
      _node = _new_node;
    }
  }

  _node->data = _copy_func(data);
  return _node->data;
}

bool PathNode::delete_node(const std::string& path){
  _node_data* _node = _get_node_data(path);
  if(!_node)
    return false;

  _remove_node(_node);
  return true;
}


void* PathNode::get_node_data(const std::string& path) const{
  _node_data* _node = _get_node_data(path);
  if(!_node)
    return NULL;

  return _node->data;
}

bool PathNode::has_node(const std::string& path) const{
  return _get_node_data(path);
}
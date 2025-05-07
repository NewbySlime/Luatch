#ifndef COUNTED_OWNERSHIP_HEADER
#define COUNTED_OWNERSHIP_HEADER

#include "map"
#include "memory"


// This data structure operates like std::map, with keys and values. But it uses key counting to determine the value's lifetime. Key count is based on how many the key are inserted or removed.
template<typename T_key, typename T_value> class CountedOwnership{
  private:
    struct _value_data{
      T_value value;
      size_t key_count = 0;
    };

    std::map<T_key, _value_data*> _value_map;

  public:
    ~CountedOwnership(){
      for(auto _pair: _value_map)
        delete _pair.second;

      _value_map.clear();
    }

    // If key already exists, the existing value will not be replaced. But the key count will be incremented.
    void insert(T_key key, T_value value){
      _value_data* _result = NULL;

      auto _iter = _value_map.find(key);
      if(_iter == _value_map.end()){
        _result = new _value_data();
        _result->key_count = 0; // will be incremented later
        _result->value = value;

        _value_map[key] = _result;
      }
      else
        _result = _iter->second;

      _result->key_count++;
    }

    void remove(T_key key){
      auto _iter = _value_map.find(key);
      if(_iter == _value_map.end())
        return;

      _iter->second->key_count--;
      if(_iter->second->key_count == 0){
        delete _iter->second;
        _value_map.erase(key);
      }
    }


    bool is_key_exists(T_key key) const{
      return _value_map.find(key) != _value_map.end();
    }


    size_t get_key_count(T_key key) const{
      auto _iter = _value_map.find(key);
      if(_iter == _value_map.end())
        return 0;

      return _iter->second->key_count;
    }

    T_value get_value(T_key key, T_value default_value) const{
      auto _iter = _value_map.find(key);
      if(_iter == _value_map.end()){
        T_value _dmp = default_value;
        return _dmp;
      }

      return _iter->second->value;
    }
};

#endif
#ifndef JSON_FILE_HANDLE_HEADER
#define JSON_FILE_HANDLE_HEADER

#include "I_persistance_handle.h"

#include "godot_cpp/classes/json.hpp"

#include "functional"
#include "string"
#include "vector"


// Any value that is a type of PackedByteArray will be encoded to or decoded from Base64 string.
class JsonFileHandle: public IPersistanceHandle{
  private:
    godot::String _file_path;
    godot::Dictionary _data;

    void _get_dict_data(std::vector<std::string>& split_data, int current_idx, std::function<void(godot::Variant&)> cb, godot::Dictionary* current_dict = NULL);

  public:
    JsonFileHandle(const godot::String& file_path);

    godot::Dictionary& get_data();
    const godot::Dictionary& get_data() const;

    // If key has path like structure, it will create a tree like data structure.
    godot::Variant get_value(const godot::Variant& key) override;
    // If key has path like structure, it will create a tree like data structure.
    void set_value(const godot::Variant& key, const godot::Variant& value) override;

    void refresh_data() override;
    void save_data() override;
};

#endif
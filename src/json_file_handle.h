#ifndef JSON_FILE_HANDLE_HEADER
#define JSON_FILE_HANDLE_HEADER

#include "I_persistance_handle.h"

#include "godot_cpp/classes/json.hpp"

#include "string"


// Any value that is a type of PackedByteArray will be encoded to or decoded from Base64 string.
class JsonFileHandle: public IPersistanceHandle{
  private:
    godot::String _file_path;
    godot::Dictionary _data;

  public:
    JsonFileHandle(const godot::String& file_path);

    godot::Dictionary& get_data();
    const godot::Dictionary& get_data() const;

    godot::Variant get_value(const godot::Variant& key) override;
    void set_value(const godot::Variant& key, const godot::Variant& value) override;

    void refresh_data() override;
    void save_data() override;
};

#endif
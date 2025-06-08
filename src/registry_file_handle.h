// TODO
//  [ ] Use path based tree like structure
//  [ ] Move the implementation used in JsonFileHandle as TreePersistanceHandle, then reuse that class. 

#if (_WIN64) || (_WIN32)
#ifndef REGISTTRY_FILE_HANDLE_HEADER
#define REGISTTRY_FILE_HANDLE_HEADER

#include "I_persistance_handle.h"
#include "Windows.h"

#include "string"


class RegistryFileHandle: public IPersistanceHandle{
  private:
    std::string _key_path;
    godot::Dictionary _data;

    static godot::Dictionary _parse_data_from_hkey(HKEY key);
    static void _store_data_to_hkey(HKEY key, const godot::Dictionary& data);

  public:
    RegistryFileHandle(const std::string& key_path);

    godot::Variant get_value(const godot::Variant& key) override;
    void set_value(const godot::Variant& key, const godot::Variant& value) override;

    void refresh_data() override;
    void save_data() override;
};

#endif // Header def
#endif // Windows def
#ifndef APPLICATION_METADATA_HEADER
#define APPLICATION_METADATA_HEADER

#include "persistance_node.h"
#include "json_file_handle.h"
#include "registry_file_handle.h"


class ApplicationMetadata: public PersistanceNode{
  GDCLASS(ApplicationMetadata, PersistanceNode)

  private:
#if (_WIN64) || (_WIN32)
    RegistryFileHandle* _reg_handle = NULL;
#elif (__linux)
    JsonFileHandle* _reg_handle = NULL;
#endif

    bool _is_loaded = false;

    void _set_data(const godot::Variant& key, const godot::Variant& value) override;
    godot::Variant _get_data(const godot::Variant& key) override;

    void _save_data() override;
    void _load_data() override;

    bool _is_data_loaded() override;

  protected:
    static void _bind_methods();

  public:
    ~ApplicationMetadata();

    void _ready() override;
};

#endif
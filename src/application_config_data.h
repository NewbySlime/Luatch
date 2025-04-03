#ifndef APPLICATION_CONFIG_DATA_HEADER
#define APPLICATION_CONFIG_DATA_HEADER

#include "json_file_handle.h"
#include "persistance_node.h"


class ApplicationConfigData: public PersistanceNode{
  GDCLASS(ApplicationConfigData, PersistanceNode)

  public:
    // Param:
    //  - STRING: key of modified value
    static const char* s_data_changed;

    static const char* default_option_config_file_path;

  private:
    JsonFileHandle* _file_handle = NULL;
    godot::Dictionary _default_data;

    godot::String _config_file_path = default_option_config_file_path;
    bool _is_loaded = false;

    void _set_data(const godot::Variant& key, const godot::Variant& value) override;
    godot::Variant _get_data(const godot::Variant& key) override;

    void _save_data() override;
    void _load_data() override;

    bool _is_data_loaded() override;

    // will also fix the target data to have the intended values.
    // returns true if there are missing values.
    bool _check_missing_data(godot::Dictionary& target, const godot::Dictionary& comparison);

  protected:
    static void _bind_methods();

  public:
    ~ApplicationConfigData();
  
    void _ready() override;

    void set_config_file_path(const godot::String& path);
    godot::String get_config_file_path() const;

    void set_default_data(const godot::Dictionary& data);
    godot::Dictionary get_default_data() const;
};

#endif
#ifndef I_PERSISTANCE_HANDLE_HEADER
#define I_PERSISTANCE_HANDLE_HEADER

#include "godot_cpp/variant/variant.hpp"


class IPersistanceHandle{
  public:
    virtual godot::Variant get_value(const godot::Variant& key) = 0;
    virtual void set_value(const godot::Variant& key, const godot::Variant& value) = 0;

    virtual void refresh_data() = 0;
    virtual void save_data() = 0;
};

#endif
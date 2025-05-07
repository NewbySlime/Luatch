#ifndef COUNTED_OBJECT_LIFETIME_NODE_HEADER
#define COUNTED_OBJECT_LIFETIME_NODE_HEADER

#include "counted_ownership.h"
#include "object_lifetime.h"

#include "godot_cpp/classes/node.hpp"


class CountedObjectLifetimeNode: public godot::Node, public CountedOwnership<godot::Variant, ObjectLifetime>{
  GDCLASS(CountedObjectLifetimeNode, godot::Node)

  protected:
    static void _bind_methods();

};

#endif
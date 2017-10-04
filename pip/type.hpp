#pragma once

#include <cc/node.hpp>

namespace pip
{
  // The kinds of types.
  enum type_kind : int
  {
    tk_int,
    tk_range,
    tk_wild,
  };

  /// Represents a type.
  struct type : cc::node
  {
    type(type_kind k)
      : cc::node(k, {}) 
    { }
  };

  /// The type of integer values. 
  ///
  /// TODO: I think we also need to represent integer encoding.
  struct int_type : type
  {
    int_type(int w)
      : type(tk_int), width(w) 
    { }
    
    /// The width (precision) of integer values.
    int width;
  };

  /// A range over an integer type.
  struct range_type : type
  {
    range_type(type* t)
      : type(tk_range), value(t) 
    { }

    /// The underlying integer type.
    type* value;
  };

  /// The type of wildcard values.
  struct wild_type : type
  {
    wild_type(int w)
      : type(tk_wild), width(w) 
    { }
  
    /// The width (precision) of wildcard values.
    int width;
  };  

// -------------------------------------------------------------------------- //
// Operations

  /// Returns the name of a type node.
  const char* get_node_name(type_kind k);

  // Returns the kind of a type.
  inline type_kind
  get_type_kind(const type* t)
  {
    return static_cast<type_kind>(t->kind);
  }


  /// Returns the node name of a program.
  inline const char* 
  get_node_name(const type* t)
  {
    return get_node_name(get_type_kind(t));
  }

} // namespace pip

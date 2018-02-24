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
    tk_port,
    tk_loc,
    tk_ref
  };
  
  /// Represents a type.
  struct type : cc::node
  {
    type(type_kind k)
      : cc::node(k, {}) 
    { }

    virtual ~type()
    {}
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

    ~range_type()
    { delete value; }
    
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
  
  /// The type of a port expression
  struct port_type : type
  {
    port_type()
      : type(tk_port)
    { }
  };
  
  /// The type of a location or field
  struct loc_type : type
  {
    loc_type()
      : type(tk_loc)
    { }
  };

  struct ref_type : type
  {
    ref_type()
      : type(tk_ref)
    { }
  };
  
// -------------------------------------------------------------------------- //
// Operations
  
  /// Returns the name of a type node.
  inline const char* get_node_name(type_kind k)
  {
    switch(k)
    {
    case tk_int:
      return "int type";
    case tk_range:
      return "range type";
    case tk_wild:
      return "wildcard type";
    case tk_port:
      return "port type";
    case tk_loc:
      return "location type";
    case tk_ref:
      return "reference type";
    }
  }
  
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

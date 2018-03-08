#pragma once

#include <pip/syntax.hpp>
#include <pip/type.hpp>

#include <cstdint>
#include <unordered_map>

// Expressions are operands of match criteria and actions. Currently, we
// only support literal values and references to declarations in these
// positions. In the future, we might extend this to support a broader
// range of constant or even runtime-computable expressions (depending
// the target architecture).

namespace pip
{
  // The kinds of expressions.
  enum expr_kind : int
  {
    ek_int,   // Integer literals
    ek_range, // Range literals
    ek_wild,  // Wildcard literals
    ek_miss,  // Miss literal.
    ek_ref,   // Declaration reference
    ek_named_field, // A named field, for example eth.ethertype
    ek_port, // Port number
    ek_bitfield // A bitfield in the context or packet
  };

  enum address_space : int
  {
    as_packet,
    as_header,
    as_key,
    as_meta
  };
  
  /// The base class of all expressions.
  struct expr : cc::node
  {
    expr(expr_kind k, type* t)
      : cc::node(k, {}), ty(t) { }
    
    /// The type of the expression.
    type* ty;

    virtual ~expr() {}
  };
  
  /// An integer literal.
  struct int_expr : expr
  {
    int_expr(type* t, int n)
      : expr(ek_int, t), val(n)
    { }
    
    std::uint64_t val;

    ~int_expr()
    { delete static_cast<int_type*>(ty); }
  };
  
  // A range literal denoting values in the range [lo, hi].
  struct range_expr : expr
  {
    range_expr(type* t, int l, int h)
      : expr(ek_range, t), lo(l), hi(h)
    { delete ty; }
    
    int lo;
    int hi;

    ~range_expr() { delete static_cast<range_type*>(ty); }
  };
  
  expr_kind get_kind(const expr* e);

  enum reserved_ports
  {
    rp_non_reserved,
    rp_all,
    rp_controller,
    rp_table,
    rp_in_port,
    rp_any,
    rp_unset,
  };

  const std::unordered_map<std::string, reserved_ports>
  port_names {
    {"all", rp_all},
    {"controller", rp_controller},
    {"table", rp_table},
    {"in_port", rp_in_port},
    {"any", rp_any},
    {"unset", rp_unset},
  };

  // A numbered port.
  struct port_expr : expr
  {
    port_expr(type* t, expr* p)
      :expr(ek_port, t), port_num(p), rp(rp_non_reserved)
    {
    }

    port_expr(type* t, symbol* port_name)
      :expr(ek_port, t), port_num()
    {
      std::string name = *port_name;
      auto it = port_names.find(name);
      rp = it->second;
    }

    ~port_expr() { delete static_cast<port_type*>(ty); }

    expr* port_num;
    reserved_ports rp;
  };

  /// A wildcard literal denoting all values k satisfying
  /// `k & ~mask == val`.
  ///
  /// TODO: Make sure that this is correct.
  struct wild_expr : expr
  {
    wild_expr(type* t, int n, int m)
      : expr(ek_wild, t), val(n), mask(m)
    { }
    
    int val;
    int mask;

    ~wild_expr() { delete static_cast<wild_type*>(ty); }
  };
  
  /// The 'miss' literal.
  struct miss_expr : expr
  {
    miss_expr(type* t)
      : expr(ek_miss, t)
    {}

    ~miss_expr()
    {
      if(int_type* miss_ty = dynamic_cast<int_type*>(ty))
	 delete miss_ty;
    }
  };
  
  /// A reference to a declared value. Note that references are resolved
  /// in a second pass.
  struct ref_expr : expr
  {
    ref_expr(type* t, symbol* id)
      : expr(ek_ref, t), id(id)
    { throw std::runtime_error("Unimplemented."); }
    
    /// The identifier naming the table.
    symbol* id;

    /// The referenced declaration.
    decl* ref;

    ~ref_expr() { delete ty;}
  };
 
  
  struct bitfield_expr : expr
  {
    bitfield_expr(type* t, address_space as, expr* pos, expr* len)
      : expr(ek_bitfield, t), as(as), pos(pos), len(len)
    { }
    
    address_space as;
    expr* pos;
    expr* len;

    ~bitfield_expr() { delete static_cast<loc_type*>(ty); }
  };

  /// A reference to a packet header.
  ///
  /// \todo These probably resolve to a function that can load the value
  /// from a packet.
  struct named_field_expr : expr
  {
    named_field_expr(expr_kind k, type* t)
      : expr(k, t)
    { }
    
    named_field_expr(expr_kind k, type* t, bitfield_expr* val)
      : expr(k, t)
    {
      value = val;
    }

    named_field_expr(expr_kind k, type* t, symbol* field)
      : expr(k, t), field(field)
    { }

    bitfield_expr* value;
    symbol* field;

    ~named_field_expr()
    {
      delete static_cast<loc_type*>(ty);
    }
      
  };

// -------------------------------------------------------------------------- //
// Operations

  /// Returns the kind of an expression.
  // expr_kind get_kind(const expr* e);

  /// Returns a string representation of an expression kind.
  const char* get_phrase_name(expr_kind k);

  /// Returns a string representation of an expression's name.
  const char* get_phrase_name(const expr* e);

} // namespace pip

// -------------------------------------------------------------------------- //
// Casting

namespace cc 
{
  template<>
  struct node_info<pip::int_expr>
  {
    static bool
    has_kind(const node* n) { return get_node_kind(n) == pip::ek_int; }
  };

  template<>
  struct node_info<pip::range_expr>
  {
    static bool
    has_kind(const node* n) { return get_node_kind(n) == pip::ek_range; }
  };

  template<>
  struct node_info<pip::wild_expr>
  {
    static bool
    has_kind(const node* n) { return get_node_kind(n) == pip::ek_wild; }
  };

  template<>
  struct node_info<pip::miss_expr>
  {
    static bool
    has_kind(const node* n) { return get_node_kind(n) == pip::ek_miss; }
  };

  template<>
  struct node_info<pip::ref_expr>
  {
    static bool
    has_kind(const node* n) { return get_node_kind(n) == pip::ek_ref; }
  };

  template<>
  struct node_info<pip::named_field_expr>
  {
    static bool
    has_kind(const node* n) { return get_node_kind(n) == pip::ek_named_field; }
  };

  template<>
  struct node_info<pip::port_expr>
  {
    static bool
    has_kind(const node* n) { return get_node_kind(n) == pip::ek_port; }
  };

  template<>
  struct node_info<pip::bitfield_expr>
  {
    static bool
    has_kind(const node* n) { return get_node_kind(n) == pip::ek_bitfield; }
  };

} // namespace cc

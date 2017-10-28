#pragma once

#include <pip/syntax.hpp>

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
    ek_field, // Field references
  };

  /// The base class of all expressions.
  struct expr : cc::node
  {
    expr(expr_kind k, type* t)
      : cc::node(k, {}), ty(t) { }

    /// The type of the expression.
    type* ty;
  };

  /// An integer literal.
  struct int_expr : expr
  {
	 int_expr(type* t, int n)
      : expr(ek_int, t), val(n)
    { }
    
    int val;
  };

  // A range literal denoting values in the range [lo, hi].
  struct range_expr : expr
  {
    range_expr(type* t, int l, int h)
      : expr(ek_int, t), lo(l), hi(h)
    { }
  
    int lo;
    int hi;
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
  };

  /// The 'miss' literal.
  struct miss_expr : expr
  {
    miss_expr(type* t)
      : expr(ek_miss, t)
    { }
  };

  /// A reference to a declared value. Note that references are resolved
  /// in a second pass.
  struct ref_expr : expr
  {
    ref_expr(type* t, symbol* id)
      : expr(ek_ref, t), id(id)
    { }

    /// The identifier naming the table.
    symbol* id;

    /// The referenced declaration.
    decl* ref;
  };

  /// A reference to a packet header.
  ///
  /// \todo These probably resolve to a function that can load the value
  /// from a packet.
  struct field_expr : expr
  {
    field_expr(expr_kind k, type* t)
      : expr(k, t)
    { }
  };

// -------------------------------------------------------------------------- //
// Operations

  /// Returns the kind of an expression.
  expr_kind get_kind(const expr* e);

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
  struct node_info<pip::field_expr>
  {
    static bool
    has_kind(const node* n) { return get_node_kind(n) == pip::ek_field; }
  };

} // namespace cc

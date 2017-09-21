#pragma once

#include <pip/syntax.hpp>

// TODO: We can defined a minimal language without matching expressions.
// Simply listing the kind of table and its values is sufficient. We could
// choose to provide a matching table that can be compiled into a sequence
// of sub-tables based on its expressions.

namespace pip
{
  // The kinds of expressions.
  enum expr_kind
  {
    ek_field, // Field references
    ek_int,   // Integer literals
    ek_range, // Range literals
    ek_wild,  // Wildcard literals
    ek_miss,  // Miss literal.
    ek_rel,   // Relational operations
    ek_in,    // Range inclusion
    ek_match, // Wildcard match
  };

  enum rel_op 
  {
    op_eq,
    op_ne,
    op_lt,
    op_gt,
    op_le,
    op_ge,
  };

  enum set_op
  {
    op_in,
    op_out,
  };

  enum match_op
  {
    op_match,
    op_miss,
  };

  /// The base class of all expressions.
  struct expr : cc::node
  {
    expr(expr_kind k, type* t)
      : cc::node(k, {}) { }

    /// The type of the expression.
    type* ty;
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

  /// A relational operation.
  struct rel_expr : expr
  {
    rel_expr(type* t, rel_op op, expr* e1, expr* e2)
      : expr(ek_rel, t), op(op), e1(e1), e2(e2)
    { }
    
    rel_op op;
    expr* e1;
    expr* e2;
  };

  /// A set membership expression (in, not in).
  struct set_expr : expr
  {
    set_expr(type* t, set_op op, expr* e1, expr* e2)
      : expr(ek_rel, t), op(op), e1(e1), e2(e2)
    { }

    set_op op;
    expr* e1;
    expr* e2;
  };

  /// A wildcard matching pattern.
  struct match_expr : expr
  {
    match_expr(type* t, match_op op, expr* e1, expr* e2)
      : expr(ek_rel, t), op(op), e1(e1), e2(e2)
    { }

    match_op op;
    expr* e1;
    expr* e2;
  };

// -------------------------------------------------------------------------- //
// Operations

  /// Returns the name of a type node.
  const char* get_node_name(expr_kind k);

  // Returns the kind of a type.
  inline expr_kind
  get_expr_kind(const expr* e)
  {
    return static_cast<expr_kind>(e->kind);
  }


  /// Returns the node name of a program.
  inline const char* 
  get_node_name(const expr* e)
  {
    return get_node_name(get_expr_kind(e));
  }

} // namespace pip

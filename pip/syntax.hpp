#pragma once

#include <cc/location.hpp>
#include <cc/node.hpp>
#include <cc/symbol.hpp>

#include <vector>

namespace pip
{
  // Imported types
  using cc::symbol;
  using cc::span;
  using cc::location;

  // Imported operations
  using cc::is;
  using cc::as;
  using cc::cast;

  // Types
  struct type;
  struct int_type;
  struct range_type;
  struct wild_type;
  using type_seq = std::vector<type*>;

  // Expressions
  struct expr;
  struct field_expr;
  struct int_expr;
  struct wild_expr;
  struct range_expr;
  struct rel_expr;
  struct set_expr;
  struct match_expr;
  using expr_seq = std::vector<expr*>;

  // Actions
  struct action;
  struct advance_action;
  struct copy_action;
  struct set_action;
  struct write_action;
  struct clear_action;
  struct drop_action;
  struct match_action;
  struct goto_action;
  struct output_action;
  using action_seq = std::vector<action*>;

  // Tables
  struct table;
  using table_seq = std::vector<table*>;


  /// A Pip program.
  ///
  /// \todo This needs a list of declarations. Examples are tables, groups,
  /// meters and ports. So perhaps we need declarations too.
  struct program : cc::node
  {
    program()
      : cc::node(0, {})
    { }
  };

  // Miscellaneous classes.
  class context;

} // namespace pip

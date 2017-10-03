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
  struct table_type;
  struct port_type;
  using type_seq = std::vector<type*>;

  // Expressions
  struct expr;
  struct int_expr;
  struct wild_expr;
  struct range_expr;
  struct ref_expr;
  struct field_expr;
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

  // Declarations and program structure
  struct decl;
  struct program_decl;
  struct table_decl;
  struct meter_decl;
  using decl_seq = std::vector<decl*>;

  // Table matching.
  struct match;
  using match_seq = std::vector<match*>;

  // Miscellaneous classes.
  class context;

} // namespace pip

#pragma once

#include <cc/location.hpp>
#include <cc/node.hpp>
#include <cc/symbol.hpp>

namespace pip
{
  // Imports

  // Types
  struct type;
  struct int_type;
  struct range_type;
  struct wild_type;

  // Expressions
  struct expr;
  struct field_expr;
  struct int_expr;
  struct wild_expr;
  struct range_expr;
  struct rel_expr;
  struct set_expr;
  struct match_expr;

  // Actions
  struct Action;


} // namespace pip

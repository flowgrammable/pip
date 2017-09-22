#pragma once

#include <pip/syntax.hpp>

namespace pip
{
  // Kinds of declarations.
  enum decl_kind : int
  {
    dk_table,
    dk_meter,
  };

  // Kinds of matching strategies.
  enum match_kind : int
  {
    mk_exact,
    mk_prefix,
    mk_wildcard,
    mk_range,
  };

  /// The base class of all declarations. A declaration introduces a named
  /// entity.
  struct decl : cc::node
  {
    decl(decl_kind k, symbol* id)
      : cc::node(k, {}), id(id)
    { }

    symbol* id;
  };


  /// Represents a match in a table. This is a key/value pair where the key
  /// is a literal expression, and the value is an action list expression.
  /// The matching kind of this object must match that of the table it 
  /// appears in.
  struct match
  {
    match(expr* k, action* al)
      : key(k), acts(al)
    { }

    expr* key;
    action* acts;
  };

  /// A sequence of matches.
  using match_seq = std::vector<match>;

  /// A lookup table. 
  struct table_decl : decl
  {
    table_decl(symbol* id, match_kind k)
      : decl(dk_table, id), match(k)
    { }

    match_kind match;
    action_seq prep;
    match_seq rules;
  };


  /// A flow metering device.
  struct meter_decl : decl
  {
    meter_decl(symbol* id)
      : decl(dk_meter, id)
    { }
  };

} // namespace pip

#pragma once

#include <pip/syntax.hpp>

//
// The 'exact' tag indicates a hash table. Other values would include
// 'wildcard' and 'range'. The 'key' phrase defines the key extraction 
// fragment (a sequence of reads into a key register) for lookup. The
// 
// This seems like a better design than what's captured below.

namespace pip
{
  // Match kind. This applies to both tables and matches.
  enum match_kind : int
  {
    tk_hash_table,
    tk_bound_table,
    tk_wild_table,
    tk_expr_table,
  };


  /// Represents a match in a table. This is a key/value pair where the key
  /// is a literal expression, and the value is an action list expression.
  /// The matching kind of this object must match that of the table it 
  /// appears in.
  struct match : cc::node
  {
    match(match_kind mk, expr* k, expr* l)
      : cc::node(mk, {}), key(k), actions(a)
    { }
  };

  /// The base class of all tables. Each table has a unique id and a list
  /// of matching rules.
  struct table : cc::node
  {
    table(table_kind k)
      : cc::node(k, {})
    { }

    symbol* id;
    match_seq rules;
  };

  struct hash_table : table
  {

  };

} // namespace pip

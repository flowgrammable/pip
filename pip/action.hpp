#pragma once

#include <pip/syntax.hpp>

namespace pip
{
  // The kinds of actions.
  enum action_kind
  {
  };

  /// The base class of all actions.
  struct action : cc::node
  {
    action(action_kind k)
      : cc::node(k, {})
    { }
  };


// -------------------------------------------------------------------------- //
// Operations

} // namespace pip

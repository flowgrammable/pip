#pragma once

#include <pip/syntax.hpp>

namespace pip
{
  // The kinds of actions.
  enum action_kind
  {
    // Decoding
    ak_advance,

    // Read and write
    ak_copy,
    ak_set,

    // Action list
    ak_write,
    ak_clear,

    // Terminator actions
    ak_drop,
    ak_match,
    ak_goto,
    ak_output,
  };

  /// The base class of all actions.
  struct action : cc::node
  {
    action(action_kind k)
      : cc::node(k, {})
    { }
  };

  /// Advance the decode buffer by n bytes.
  struct advance_action : action
  {
    advance_action()
      : action(ak_advance)
    { }
  };

  /// Copy a field into metadata or the key register.
  ///
  /// \todo Can we copy into the packet? Can the source be a metadata
  /// variable (e.g., ingress port)?
  struct copy_action : action
  {
    copy_action()
      : action(ak_copy)
    { }
  };

  /// Set a value of a packet field.
  ///
  /// \todo Can we set a metadata variable?
  struct set_action : action
  {
    set_action()
      : action(ak_set)
    { }
  };

  /// Write an action to the action list.
  struct write_action : action
  {
    write_action(action* a)
      : action(ak_write)
    { }

    action* act;
  };

  /// Clear the action list.
  struct clear_action : action
  {
    clear_action()
      : action(ak_clear)
    { }
  };

  /// Drop the packet and terminate execution.
  struct drop_action : action
  {
    drop_action()
      : action(ak_copy)
    { }
  };

  /// Perform a lookup in the given table and execute its associated 
  /// action list. This is the terminator for the key reading stage
  /// of table lookup.
  struct match_action : action
  {
    match_action()
      : action(ak_copy)
    { }
  };

  /// Transition to the next processor (table, meter, group?).
  struct goto_action : action
  {
    goto_action(expr* e)
      : action(ak_copy)
    { }

    /// The destination expression.
    expr* dest;
  };

  /// Send the packet to a destination port.
  struct output_action : action
  {
    output_action(expr* p)
      : action(ak_copy), port(p)
    { }

    expr* port;
  };


// -------------------------------------------------------------------------- //
// Operations

  /// Returns the kind of an action.
  inline action_kind 
  get_kind(const action* a)
  {
    return static_cast<action_kind>(a->kind);
  }

} // namespace pip


// -------------------------------------------------------------------------- //
// Casting

namespace cc 
{
  template<>
  struct node_info<pip::advance_action>
  {
    static bool
    has_kind(const node* n) { return get_node_kind(n) == pip::ak_advance; }
  };

  template<>
  struct node_info<pip::copy_action>
  {
    static bool
    has_kind(const node* n) { return get_node_kind(n) == pip::ak_copy; }
  };

  template<>
  struct node_info<pip::set_action>
  {
    static bool
    has_kind(const node* n) { return get_node_kind(n) == pip::ak_set; }
  };

  template<>
  struct node_info<pip::write_action>
  {
    static bool
    has_kind(const node* n) { return get_node_kind(n) == pip::ak_write; }
  };

  template<>
  struct node_info<pip::clear_action>
  {
    static bool
    has_kind(const node* n) { return get_node_kind(n) == pip::ak_clear; }
  };

  template<>
  struct node_info<pip::drop_action>
  {
    static bool
    has_kind(const node* n) { return get_node_kind(n) == pip::ak_drop; }
  };

  template<>
  struct node_info<pip::match_action>
  {
    static bool
    has_kind(const node* n) { return get_node_kind(n) == pip::ak_match; }
  };

  template<>
  struct node_info<pip::goto_action>
  {
    static bool
    has_kind(const node* n) { return get_node_kind(n) == pip::ak_goto; }
  };

  template<>
  struct node_info<pip::output_action>
  {
    static bool
    has_kind(const node* n) { return get_node_kind(n) == pip::ak_output; }
  };

} // namespace cc

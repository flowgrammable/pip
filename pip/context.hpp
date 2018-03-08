#pragma once

#include <pip/syntax.hpp>

#include <cc/diagnostics.hpp>
#include <cc/factory.hpp>
#include <cc/input.hpp>

namespace pip
{
  /// The context provides access to a number of useful translation facilities
  /// and manages memory for expressions.
  class context
  {
  public:
    context(cc::diagnostic_manager& diags, 
	    cc::input_manager& in, 
	    cc::symbol_table& syms);
    ~context();
    
    /// Returns the diagnostic manager.
    cc::diagnostic_manager& get_diagnostics() { return diags; }

    /// Returns the input manager.
    cc::input_manager& get_input() const { return input; }
    
    /// Returns the associated lexical context.
    cc::symbol_table& get_symbol_table() { return syms; }

    /// Returns a unique symbol.
    symbol* get_symbol(const char* str);
    symbol* get_symbol(const std::string& str);

    // TOOD: Add factories for creating terms, e.g., make_program.
  public:
    expr* make_int_expr(type* t, int val);
    expr* make_range_expr(type* t, int lo, int hi);
    expr* make_wild_expr(type* t, int val, int mask);
    expr* make_miss_expr(type* t);
    expr* make_ref_expr(type* t, symbol* id);
    expr* make_named_field_expr(type* t, symbol* field);
    expr* make_port_expr(type* t, expr* port_num);
    expr* make_port_expr(type* t, symbol* port_name);
    expr* make_bitfield_expr(address_space space, expr* pos, expr* len);

    action* make_advance_action(expr* amount);
    action* make_copy_action(expr* src, expr* dst, expr* n);
    action* make_set_action(expr* f, expr* v);
    action* make_write_action(action* act);
    action* make_clear_action();
    action* make_drop_action();
    action* make_match_action();
    action* make_goto_action(expr* table);
    action* make_output_action(expr* p);
    
  private:
    /// The diagnostic manager.
    cc::diagnostic_manager& diags;
    
    /// The input manager.
    cc::input_manager& input;

    /// The symbol table.
    cc::symbol_table& syms;

    /// Allocated memory for expressions and actions (not limited).
    /// Their memory goes out of scope with the context.
    std::vector<expr*> expression_pool;
    std::vector<action*> action_pool;
  };

} // namespace pip

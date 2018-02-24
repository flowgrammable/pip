#include "context.hpp"
#include "expr.hpp"
#include "action.hpp"
#include "type.hpp"

namespace pip
{
  context::context(cc::diagnostic_manager& diags, 
		   cc::input_manager& in, 
		   cc::symbol_table& syms)
    : diags(diags), input(in), syms(syms)
  { }

  context::~context()
  {
    for(auto expression : expression_pool)
      delete expression;
    for(auto act : action_pool)
      delete act;
  }
  
  symbol*
  context::get_symbol(const char* str)
  {
    return syms.get(str);
  }
  
  symbol*
  context::get_symbol(const std::string& str)
  {
    return syms.get(str);
  }
  
  expr*
  context::make_int_expr(type* t, int val)
  {
    expression_pool.emplace_back(new int_expr(t, val));
    return expression_pool.back();
  }
  
  expr*
  context::make_range_expr(type* t, int lo, int hi)
  {    
    expression_pool.emplace_back(new range_expr(t, lo, hi));
    return expression_pool.back();
  }
  
  expr*
  context::make_wild_expr(type* t, int val, int mask)
  {
    expression_pool.emplace_back(new wild_expr(t, val, mask));
    return expression_pool.back();
  }
  
  expr*
  context::make_miss_expr(type* t)
  {
    expression_pool.emplace_back(new miss_expr(t));
    return expression_pool.back();
  }
  
  expr*
  context::make_ref_expr(type* t, symbol* id)
  {
    expression_pool.emplace_back(new ref_expr(t, id));
    return expression_pool.back();
  }
  
  expr*
  context::make_named_field_expr(type* t, symbol* field)
  {
    expression_pool.emplace_back(new named_field_expr(ek_named_field, t, field));
    return expression_pool.back();
  }
  
  expr*
  context::make_port_expr(type* t, expr* port_num)
  {
    expression_pool.emplace_back(new port_expr(t, port_num));
    return expression_pool.back();
  }

  expr*
  context::make_bitfield_expr(address_space space, expr* pos, expr* len)
  {
    expression_pool.emplace_back(new bitfield_expr(new loc_type, space, pos, len));
    return expression_pool.back();
  }
  
  action*
  context::make_advance_action(expr* amount)
  {
    action_pool.emplace_back(new advance_action(amount));
    return action_pool.back();
  }
  
  action*
  context::make_copy_action(expr* src, expr* dst, expr* n)
  {
    action_pool.emplace_back(new copy_action(src, dst, static_cast<int_expr*>(n)));
    return action_pool.back();
  }

  action*
  context::make_set_action(expr* f, expr* v)
  {
    action_pool.emplace_back(new set_action(f, v));
    return action_pool.back();
  }

  action*
  context::make_write_action(action* act)
  {
    action_pool.emplace_back(new write_action(act));
    return action_pool.back();
  }

  action*
  context::make_clear_action()
  {
    action_pool.emplace_back(new clear_action());
    return action_pool.back();
  }
  
  action*
  context::make_drop_action()
  {
    action_pool.emplace_back(new drop_action());
    return action_pool.back();
  }
  
  action*
  context::make_match_action()
  {
    action_pool.emplace_back(new match_action());
    return action_pool.back();
  }
  
  action*
  context::make_goto_action(expr* table)
  {
    action_pool.emplace_back(new goto_action(table));
    return action_pool.back();
  }
  
  action*
  context::make_output_action(expr* p)
  {
    action_pool.emplace_back(new output_action(p));
    return action_pool.back();
  }  
}
  
  

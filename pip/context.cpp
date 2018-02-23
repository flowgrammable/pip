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
    return new int_expr(t, val);
  }
  
  expr*
  context::make_range_expr(type* t, int lo, int hi)
  {
    return new range_expr(t, lo, hi);
  }
  
  expr*
  context::make_wild_expr(type* t, int val, int mask)
  {
    return new wild_expr(t, val, mask);
  }
  
  expr*
  context::make_miss_expr(type* t)
  {
    return new miss_expr(t);
  }
  
  expr*
  context::make_ref_expr(type* t, symbol* id)
  {
    return new ref_expr(t, id);
  }
  
  expr*
  context::make_field_expr(type* t, symbol* field)
  {
    return new field_expr(ek_field, t, field);
  }
  
  expr*
  context::make_port_expr(type* t, expr* port_num)
  {
    return new port_expr(t, port_num);
  }

  expr*
  context::make_offset_expr(symbol* space, expr* pos, expr* len)
  {
    return new offset_expr(new loc_type, space, pos, len);
  }
  

  action*
  context::make_action(action_kind k, expr* parm_a, expr* parm_b, action* a)
  {
    throw std::runtime_error("Do not use this make_action function.\n");
    switch(k) {
    case ak_advance:
      return new advance_action(parm_a);
    case ak_copy:
      throw std::runtime_error("Use of deprecated make_action function >:(\n");
    case ak_set:
      return new set_action(parm_a, parm_b);
    case ak_write:
      return new write_action(a);
    case ak_clear:
      return new clear_action;
    case ak_drop:
      return new drop_action;
    case ak_match:
      return new match_action;
    case ak_goto:
      return new goto_action(parm_a);
    case ak_output:
      return new output_action(parm_a);
    default:
      throw std::runtime_error("Unexpected action\n");
    }
  }

  action* context::make_advance_action(expr* amount) const
  {
    return new advance_action(amount);
  }
  
  action* context::make_copy_action(expr* src, expr* dst, expr* n) const
  {
    return new copy_action(src, dst, static_cast<int_expr*>(n));
  }

  action* context::make_set_action(expr* f, expr* v) const
  {
    return new set_action(f, v);
  }

  action* context::make_write_action(action* act) const
  {
    return new write_action(act);
  }

  action* context::make_clear_action() const
  {
    return new clear_action();
  }
  
  action* context::make_drop_action() const
  {
    return new drop_action();
  }
  
  action* context::make_match_action() const
  {
    return new match_action();
  }
  
  action* context::make_goto_action(expr* table) const
  {
    return new goto_action(table);
  }
  
  action* context::make_output_action(expr* p) const
  {
    return new output_action(p);
  }
}
  
  

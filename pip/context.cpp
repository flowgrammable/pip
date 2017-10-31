#include "context.hpp"
#include "expr.hpp"
#include "action.hpp"

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
	context::make_field_expr(type* t, expr_kind kind)
	{
		return new field_expr(kind, t);
	}

	expr*
	context::make_port_expr(type* t, int port_num)
	{
		return new port_expr(t, port_num);
	}

	action*
	context::make_action(action_kind k, expr* parm_a, expr* parm_b, action* a)
	{
		switch(k) {
		case ak_advance:
			return new advance_action;
		case ak_copy:
			return new copy_action(parm_a, parm_b);
		case ak_set:
			return new set_action;
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
	

} // namesapce pip

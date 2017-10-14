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

	action*
	context::make_action(action_kind k)
	{
		return new action(k);
	}
	

} // namesapce pip

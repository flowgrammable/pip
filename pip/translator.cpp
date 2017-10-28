#include "translator.hpp"
#include "decl.hpp"
#include "type.hpp"
#include "context.hpp"
#include "expr.hpp"
#include "action.hpp"
#include "dumper.hpp"

#include <string>

// for testing only
#include <iostream>
#include <sstream>

namespace pip
{
  translator::translator(context& cxt)
    : cxt(cxt)
  { }

  /// program ::= (pip <decl-seq>)
  decl*
  translator::trans_program(const sexpr::expr* e)
  {
    if (const sexpr::list_expr* list = as<sexpr::list_expr>(e)) {
      decl_seq decls;
      match_list(list, "pip", &decls);
			
      return new program_decl(std::move(decls));
    }
    sexpr::throw_unexpected_term(e);
  }

  /// decl-seq ::= (<decl*>)
  decl_seq
  translator::trans_decls(const sexpr::expr* e)
  {
    if (const sexpr::list_expr* list = as<sexpr::list_expr>(e)) {
      decl_seq decls;
      // for (const sexpr::expr* elem : list->exprs) {
        decl* d = trans_decl(list);
        decls.push_back(d);
      // }
      return decls;
    }
    sexpr::throw_unexpected_term(e);
  }

  /// decl ::= table-decl | meter-decl
  decl*
  translator::trans_decl(const sexpr::expr* e)
  {
    if (const sexpr::list_expr* list = as<sexpr::list_expr>(e)) {
      symbol* sym;
      match_list(list, &sym);
      if (*sym == "table")
        return trans_table(list);
      sexpr::throw_unexpected_id(cast<sexpr::id_expr>(list->exprs[0]));
    }
		sexpr::throw_unexpected_term(e);
  }

  /// table-decl ::= (table id <match-kind> <match-seq>)
  ///
  /// rule-kind ::= exact | prefix | wildcard | range
  ///
  /// FIXME: We actually need parse the key extraction program. This
  /// would be a sequence of actions that bits from the current offset
  /// into a "key register".
  decl*
  translator::trans_table(const sexpr::list_expr* e)
  {
    symbol* id;
    symbol* kind;
		action_seq actions;
    rule_seq rules;
    match_list(e, "table", &id, &kind, &actions, &rules);

    // TODO: Actually parse the kind of match. For now, we can simply
    // assume that all tables are exact. Hint: use the match_list framework
    // to return a match kind.

    return new table_decl(id, rk_exact, std::move(actions), std::move(rules));
  }

  /// rule_seq ::= (<rule*>)
  rule_seq
  translator::trans_rules(const sexpr::expr* e)
  {
    if (const sexpr::list_expr* list = as<sexpr::list_expr>(e)) {
      rule_seq rules;

			// IN PROGRESS: Match each element in turn.
			rule* r = trans_rule(e);
			rules.push_back(r);
      
      return rules;
    }
    sexpr::throw_unexpected_term(e);    
  }

	rule*
	translator::trans_rule(const sexpr::expr* e)
	{
    if (const sexpr::list_expr* list = as<sexpr::list_expr>(e)) {
			expr_seq exprs;
			expr* key;
			action_seq actions;

			match_list(list, "rule", &key, &actions);

			return new rule(rk_exact, key, std::move(actions));
		}
		
		sexpr::throw_unexpected_term(e);
	}

	action_seq
	translator::trans_actions(const sexpr::expr* e)
	{
    if (const sexpr::list_expr* list = as<sexpr::list_expr>(e)) {
      action_seq actions;
			action* a = trans_action(list);
			actions.push_back(a);
      return actions;
    }
    sexpr::throw_unexpected_term(e);
	}

	// TODO: parse extra information (i.e. expr to output for output_action).
	action*
	translator::trans_action(const sexpr::list_expr* e)
	{
		symbol* action_name;
		expr_seq exprs;
		int i;

		match_list(e, &action_name);
		
		if(*action_name == "advance")
			return cxt.make_action(ak_advance);

		if(*action_name == "copy")
			return cxt.make_action(ak_copy);
		if(*action_name == "set")
			return cxt.make_action(ak_set);

		if(*action_name == "write")
			return cxt.make_action(ak_write);
		if(*action_name == "clear")
			return cxt.make_action(ak_clear);

		if( *action_name == "drop" )
			return cxt.make_action(ak_drop);
		if( *action_name == "match" )
			return cxt.make_action(ak_match);
		if( *action_name == "goto" )
			return cxt.make_action(ak_goto);
		if( *action_name == "output" )
			return cxt.make_action(ak_output);
			
		sexpr::throw_unexpected_term(e);
	}

	expr*
	translator::trans_expr(const sexpr::expr* e)
	{
		// Match bare integer literals.
		// TODO: match int width
    // if (const sexpr::int_expr* num = as<sexpr::int_expr>(e))
    //   return trans_int_expr(num, 32);

		// Match bare keyword literals.
    if (const sexpr::id_expr* id = as<sexpr::id_expr>(e)) {
      if (*id->id == "miss")
				return trans_miss_expr(id);
		}

		// Match phrases.
    //
    // FIXME: Use a lookup table + switch.
    if (const sexpr::list_expr* list = as<sexpr::list_expr>(e)) {
      symbol* sym;
      match_list(list, &sym);
			if (*sym == "int")
				return trans_int_expr(list);
			
			if (*sym == "wildcard")
        return trans_wild_expr(list);
			if (*sym == "range")
				return trans_range_expr(list);
			if (*sym == "port");
			// return trans_port_expr(list);
		}
	}

	expr_seq
	translator::trans_exprs(const sexpr::expr* e)
	{
    if (const sexpr::list_expr* list = as<sexpr::list_expr>(e)) {
      expr_seq exprs;
      for (const sexpr::expr* elem : list->exprs) {
        expr* e = trans_expr(elem);
        exprs.push_back(e);
      }
      return exprs;
    }
    sexpr::throw_unexpected_term(e);
	}
	
	expr*
	translator::trans_range_expr(const sexpr::list_expr* e)
	{
		// int lo, hi;
		// symbol* lo_type, * hi_type;
		expr* lo;
		expr* hi;
    match_list(e, "range", &lo, &hi);

		// TODO: proper error handling
		// int lo_width, hi_width;
		// if(lo_width = deduce_int_type_width(lo_type))
		// 	if(hi_width = deduce_int_type_width(hi_type))
		// 		if(lo_width == hi_width)
		// 			return cxt.make_range_expr(
		// 				new range_type(new int_type(lo_width)), lo, hi );

		// auto lo_ty = as<int_type>(lo->ty);
		// auto hi_ty = as<int_type>(hi->ty);

		auto lo_expr = as<int_expr>(lo);
		auto hi_expr = as<int_expr>(hi);

		auto lo_val = lo_expr->val;
		auto hi_val = hi_expr->val;

		auto lo_ty = static_cast<int_type*>(lo_expr->ty);
		auto hi_ty = static_cast<int_type*>(hi_expr->ty);

		if(lo_ty->width == hi_ty->width)
			return cxt.make_range_expr(new range_type(
																	 new int_type(lo_ty->width)), lo_val, hi_val);
		//TODO: proper diagnostic
		std::stringstream ss;
		ss << "Width of range arguments not equal: " << lo_ty->width << ", and "
			 << hi_ty->width << "\n";
		throw std::runtime_error(ss.str().c_str());
	}
	
	expr*
	translator::trans_wild_expr(const sexpr::list_expr* e)
	{
	}
	
	expr*
	translator::trans_miss_expr(const sexpr::id_expr* e)
	{
	}
	
	expr*
	translator::trans_ref_expr(const sexpr::id_expr* e)
	{
	}
	
	expr*
	translator::trans_field_expr(const sexpr::id_expr* e)
	{
	}

	// When given an integer width specifier, such as i32,
	// return the integer after the i. In this case, 32.
	static int
	deduce_int_type_width(const symbol* ty)
	{
		std::string spec = *ty;
		
		if(spec.front() == 'i') {
			spec.erase(0, 1);

			std::size_t it;
			auto width = std::stoi(spec.c_str(), &it);

			if(spec.size() == it)
				return width;
		}

		std::stringstream ss;
		ss << "Invalid integer width specifier: " << *ty <<
			". Specifier should be of form i[integer].\n";
		throw std::runtime_error(ss.str().c_str());
	}
	
	expr*
	translator::trans_int_expr(const sexpr::list_expr* e)
	{
		symbol* width_specifier;
		int value;
		match_list(e, "int", &width_specifier, &value);

		int w = deduce_int_type_width(width_specifier);

		return cxt.make_int_expr( new int_type(w), value );
	}
	

// -------------------------------------------------------------------------- //
// Matching

  void 
  translator::match(const sexpr::list_expr* list, int n, decl_seq* decls)
  {
    *decls = trans_decls(get(list, n));
  }

  void 
  translator::match(const sexpr::list_expr* list, int n, rule_seq* rules)
  {
    *rules = trans_rules(get(list, n));
  }

	void 
  translator::match(const sexpr::list_expr* list, int n, expr_seq* exprs)
  {
    *exprs = trans_exprs(get(list, n));
  }

	void
	translator::match(const sexpr::list_expr* list, int n, expr** out)
	{
		*out = trans_expr(get(list, n));
	}
	
	void 
  translator::match(const sexpr::list_expr* list, int n, action_seq* actions)
  {
    *actions = trans_actions(get(list, n));
  }

} // namespace pip

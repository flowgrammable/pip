#include "translator.hpp"
#include "decl.hpp"
#include "type.hpp"
#include "context.hpp"
#include "expr.hpp"
#include "action.hpp"

// for testing only
#include <iostream>

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

    return new table_decl(id, rk_exact, std::move(rules));
  }

  /// rule_seq ::= (<rule*>)
  rule_seq
  translator::trans_rules(const sexpr::expr* e)
  {
    if (const sexpr::list_expr* list = as<sexpr::list_expr>(e)) {
      rule_seq rules;

			// IN PROGRESS: Match each element in turn.
			for (const sexpr::expr* elem : list->exprs) {
				rule* r = trans_rule(elem);
				rules.push_back(r);
			}     
      
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

			match_list(list, "rule", key, &actions);


			// TODO: match_list matches an expr_seq, but rule is constructed
			// with a single expr*. Resolve this issue.
			// return new rule(rk_exact, exprs, std::move(actions));
			return new rule(rk_exact, key, std::move(actions));
		}
		
		sexpr::throw_unexpected_term(e);
	}

	action_seq
	translator::trans_actions(const sexpr::expr* e)
	{
    if (const sexpr::list_expr* list = as<sexpr::list_expr>(e)) {
      action_seq actions;
      for (const sexpr::expr* elem : list->exprs) {
        action* a = trans_action(elem);
        actions.push_back(a);
      }
      return actions;
    }
    sexpr::throw_unexpected_term(e);
	}

	// TODO: Implement me.
	action*
	translator::trans_action(const sexpr::expr* e)
	{
    if (const sexpr::id_expr* name = as<sexpr::id_expr>(e)) {
			if(*(name->id) == "advance")
				return new action(ak_advance);

			if(*(name->id) == "copy")
				return new action(ak_copy);
			if(*(name->id) == "set")
				return new action(ak_set);

			if(*(name->id) == "write")
				return new action(ak_write);
			if(*(name->id) == "clear")
				return new action(ak_clear);

			if( *(name->id) == "drop" )
				return cxt.make_action(ak_drop);
			if( *(name->id) == "match" )
				return new action(ak_match);
			if( *(name->id) == "goto" )
				return new action(ak_goto);
			if( *(name->id) == "output" )
				return new action(ak_output);
		}
		sexpr::throw_unexpected_term(e);
	}

	expr*
	translator::trans_expr(const sexpr::expr* e)
	{
		// Match bare integer literals.
    if (const sexpr::int_expr* num = as<sexpr::int_expr>(e))
      return trans_int_expr(num);

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
		// TODO: find proper way to match type??
		// TODO: figure out how match_list is supposed to work
		
		// type* t;
    // int_expr* lo;
    // int_expr* hi;
		symbol* range_kw;
		symbol* lo_type;
		symbol* lo;
		symbol* hi_type;
		symbol* hi;
    match_list(e, &range_kw, &lo_type, &lo, &hi_type, &hi);

		std::cout << *range_kw << ' ' << *lo_type << ' ' << *lo <<
			' ' << *hi_type << *hi << '\n';
		
    // return cxt.make_range_expr( new range_type(nullptr), lo->val, hi->val );
		return nullptr;
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
	
	expr*
	translator::trans_int_expr(const sexpr::int_expr* e)
	{
		return cxt.make_int_expr( new int_type(32), e->val );
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
	translator::match(const sexpr::list_expr* list, int n, expr* out)
	{
		out = trans_expr(get(list, n));
	}
	
	void 
  translator::match(const sexpr::list_expr* list, int n, action_seq* actions)
  {
    *actions = trans_actions(get(list, n));
  }

} // namespace pip

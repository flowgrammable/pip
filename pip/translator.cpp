#include "translator.hpp"
#include "decl.hpp"
#include "type.hpp"
#include "context.hpp"
#include "expr.hpp"

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
      for (const sexpr::expr* elem : list->exprs) {
        decl* d = trans_decl(elem);
        decls.push_back(d);
      }
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
  /// match-kind ::= exact | prefix | wildcard | range
  ///
  /// FIXME: We actually need parse the key extraction program. This
  /// would be a sequence of actions that bits from the current offset
  /// into a "key register".
  decl*
  translator::trans_table(const sexpr::list_expr* e)
  {
    symbol* id;
    symbol* kind;
    rule_seq rules;
    match_list(e, "table", &id, &kind, &rules);

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
			for (const sexpr::expr* elem : list->exprs) {
				// matches.push_back();
			}
      
      // TODO: Match each element in turn.
			// match_list(e, &matches);
			// trans_expr( e );
      
      return rules;
    }
    sexpr::throw_unexpected_term(e);    
  }

	// match*
	// translator::trans_match(const sexpr::expr* e)
	// {
	// }

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

} // namespace pip

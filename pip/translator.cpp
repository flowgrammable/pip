#include "translator.hpp"
#include "decl.hpp"
#include "type.hpp"
#include "context.hpp"

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
    match_seq matches;
    match_list(e, "table", &id, &kind, &matches);

    // TODO: Actually parse the kind of match. For now, we can simply
    // assume that all tables are exact. Hint: use the match_list framework
    // to return a match kind.

    return new table_decl(id, mk_exact, std::move(matches));
  }

  /// match-seq ::= (<match*>)
  match_seq
  translator::trans_matches(const sexpr::expr* e)
  {
    if (const sexpr::list_expr* list = as<sexpr::list_expr>(e)) {
      match_seq matches;
      
      // TODO: Match each element in turn.
      
      return matches;
    }
    sexpr::throw_unexpected_term(e);    
  }

	expr*
	translator::trans_expr(const sexpr::expr* e)
	{
		// Match bare integer literals.
    if (const sexpr::int_expr* num = as<sexpr::int_expr>(e))
      return trans_int_expr(num);
	}
	
	expr*
	translator::trans_range_expr(const sexpr::list_expr* e)
	{
	}
	
	expr*
	translator::trans_wild_expr(const sexpr::list_expr* e)
	{
	}
	
	expr*
	translator::trans_miss_expr(const sexpr::list_expr* e)
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
  translator::match(const sexpr::list_expr* list, int n, match_seq* matches)
  {
    *matches = trans_matches(get(list, n));
  }

} // namespace pip

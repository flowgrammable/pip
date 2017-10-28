#pragma once

#include <pip/syntax.hpp>

#include <sexpr/translation.hpp>

struct match;

namespace pip
{

  // Performs a first-pass translation of an s-expression program into a
  // StackVM program.
  class translator : public sexpr::translator<translator>
  {
    friend class sexpr::translator<translator>;
  public:
    translator(context& cxt);

    decl* operator()(const sexpr::expr* e) { return trans_program(e); }

  private:
    decl* trans_program(const sexpr::expr* e);
    decl_seq trans_decls(const sexpr::expr* e);
    decl* trans_decl(const sexpr::expr* e);
    decl* trans_table(const sexpr::list_expr* e);

    rule_seq trans_rules(const sexpr::expr* e);
    rule_seq trans_rules(const sexpr::list_expr* e);
		rule* trans_rule(const sexpr::expr* e);

		action_seq trans_actions(const sexpr::expr* e);
		action* trans_action(const sexpr::list_expr* e);

		// TODO: ensure that these use the correct sexpr types
		expr* trans_expr(const sexpr::expr* e);
		expr_seq trans_exprs(const sexpr::expr* e);
		expr* trans_range_expr(const sexpr::list_expr* e);
		expr* trans_wild_expr(const sexpr::list_expr* e);
		expr* trans_miss_expr(const sexpr::id_expr* e);
		expr* trans_ref_expr(const sexpr::id_expr* e);
		expr* trans_field_expr(const sexpr::id_expr* e);
		expr* trans_port_expr(const sexpr::list_expr* e);
		expr* trans_int_expr(const sexpr::list_expr* e);

  private:
    // Matching extensions
    using sexpr::translator<translator>::match;

    void match(const sexpr::list_expr* list, int n, decl_seq* decls);
    void match(const sexpr::list_expr* list, int n, rule_seq* rules);
		void match(const sexpr::list_expr* list, int n, expr_seq* exprs);
		void match(const sexpr::list_expr* list, int n, expr** out);
		void match(const sexpr::list_expr* list, int n, action_seq* actions);
		void match(const sexpr::list_expr* list, int n, action** a);

  private:
    context& cxt;
  };


} // namespace pip

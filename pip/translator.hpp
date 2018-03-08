#pragma once

#include <pip/syntax.hpp>
#include <pip/decoder.hpp>
#include <pip/decl.hpp>

#include <sexpr/translation.hpp>

#include <unordered_map>

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
    /// The match rule of the table currently being translated.
    rule_kind match_kind;

    rule_seq trans_rules(const sexpr::expr* e);
    rule_seq trans_rules(const sexpr::list_expr* e);
    rule* trans_rule(const sexpr::expr* e);
    rule* trans_rule(const sexpr::list_expr* e);

    action_seq trans_actions(const sexpr::expr* e);
    action* trans_action(const sexpr::list_expr* e);

    expr* trans_expr(const sexpr::expr* e);
    expr_seq trans_exprs(const sexpr::expr* e);
    expr* trans_range_expr(const sexpr::list_expr* e);
    expr* trans_wild_expr(const sexpr::list_expr* e);
    expr* trans_miss_expr();
    expr* trans_ref_expr(const sexpr::list_expr* e);
    expr* trans_named_field_expr(const sexpr::list_expr* e);
    expr* trans_port_expr(const sexpr::list_expr* e);
    expr* trans_reserved_port_expr(const sexpr::list_expr* e);
    expr* trans_bitfield_expr(const sexpr::list_expr* e);
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
    decoder field_decoder;

  /// Various lookup tables for different symbols.
  private:
    const std::unordered_map<symbol*, address_space> address_spaces {
      {cxt.get_symbol("packet"), as_packet},
      {cxt.get_symbol("header"), as_header},
      {cxt.get_symbol("key"), as_key},
      {cxt.get_symbol("meta"), as_meta},
    };

    /// Keywords in expression lists.
    enum expression_symbol : int {
      es_int,
      es_wildcard,
      es_range,
      es_port,
      es_reserved_port,
      es_bitfield,
      es_miss,
      es_named_field,
      es_ref
    };

    const std::unordered_map<symbol*, expression_symbol> expression_symbols {
      {cxt.get_symbol("int"), es_int},
      {cxt.get_symbol("wildcard"), es_wildcard},
      {cxt.get_symbol("range"), es_range},
      {cxt.get_symbol("port"), es_port},
      {cxt.get_symbol("reserved_port"), es_reserved_port},
      {cxt.get_symbol("bitfield"), es_bitfield},
      {cxt.get_symbol("miss"), es_miss},
      {cxt.get_symbol("named_field"), es_named_field},
      {cxt.get_symbol("ref"), es_ref},
    };

    const std::unordered_map<symbol*, rule_kind> match_kinds {
      {cxt.get_symbol("exact"), rk_exact},
      {cxt.get_symbol("prefix"), rk_prefix},
      {cxt.get_symbol("wildcard"), rk_wildcard},
      {cxt.get_symbol("range"), rk_range},
    };

  };


} // namespace pip

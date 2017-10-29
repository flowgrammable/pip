#pragma once

#include <pip/syntax.hpp>

#include <cc/dump.hpp>

namespace pip
{
  /// The AST dumper is responsible for dumping the internal representation
  /// of a source tree to an output stream.
  class dumper : public cc::dumper
  {
  public:
    using cc::dumper::dumper;

    void operator()(const decl* e) { return dump_decl(e); }
    void operator()(const action* a) { return dump_action(a); }
    void operator()(const expr* e) { return dump_expr(e); }
    void operator()(const type* t) { return dump_type(t); }
		void operator()(const rule* r) { return dump_match(r); }

  private:
    void dump_decl(const decl* d);
    void dump_decl(const program_decl* d);
    void dump_decl(const table_decl* d);

    void dump_actions(const char* name, const action_seq& as);
    void dump_action(const action* a);
    void dump_action(const advance_action* a);
    void dump_action(const copy_action* a);
    void dump_action(const set_action* a);
    void dump_action(const write_action* a);
    void dump_action(const clear_action* a);
    void dump_action(const drop_action* a);
    void dump_action(const match_action* a);
    void dump_action(const goto_action* a);
    void dump_action(const output_action* a);

    void dump_matches(const char* name, const rule_seq& rs);
    void dump_match(const rule* r);

    void dump_expr(const expr* e);

    void dump_type(const type* t);
  };

} // namespace pip

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

  private:
    void dump_decl(const decl* d);
    void dump_decl(const program_decl* d);
    void dump_decl(const table_decl* d);

    void dump_action(const action* a);

    void dump_expr(const expr* e);

    void dump_type(const type* t);
  };

} // namespace pip

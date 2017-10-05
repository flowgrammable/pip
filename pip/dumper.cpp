#include "dumper.hpp"
#include "decl.hpp"
#include "action.hpp"
#include "expr.hpp"
#include "type.hpp"

#include <iostream>

namespace pip
{
  void
  dumper::dump_decl(const decl* d)
  {
    switch (get_kind(d)) {
      case dk_program:
        return dump_decl(cast<program_decl>(d));
      case dk_table:
        return dump_decl(cast<table_decl>(d));
      case dk_meter:
        // FIXME: Implement me?
        break;
    }
    throw std::logic_error("invalid declaration");
  }

  void
  dumper::dump_decl(const program_decl* p)
  {
    dump_guard g(*this, p, "pip");
    indent();
    print_newline();
    for (const decl* d : p->decls)
      dump_decl(d);
    undent();
  }

  void
  dumper::dump_decl(const table_decl* t)
  {
    dump_guard g(*this, t, "table", false);
    indent();
    print_newline();
    dump_actions("key", t->prep);
    dump_matches("rules", t->rules);
  }

  void
  dumper::dump_actions(const char* name, const action_seq& as)
  {
    print_indentation();
    print("\033[1;32m");
    print(name);
    print("\033[0m");
    indent();
    print_newline();
    for (const action* a : as)
      dump_action(a);
    undent();
  }

  void
  dumper::dump_matches(const char* name, const match_seq& ms)
  {
    print_indentation();
    print("\033[1;32m");
    print(name);
    print("\033[0m");
    indent();
    print_newline();
    for (const match* m : ms)
      dump_match(m);
    undent();
  }

  void
  dumper::dump_match(const match* m)
  {
    dump_guard g(*this, m, "match");
  }

  void
  dumper::dump_action(const action* a)
  {
    switch (get_kind(a)) {
      case ak_advance:
        return dump_action(cast<advance_action>(a));
      case ak_copy:
        return dump_action(cast<copy_action>(a));
      case ak_set:
        return dump_action(cast<set_action>(a));
      case ak_write:
        return dump_action(cast<write_action>(a));
      case ak_clear:
        return dump_action(cast<clear_action>(a));
      case ak_drop:
        return dump_action(cast<drop_action>(a));
      case ak_match:
        return dump_action(cast<match_action>(a));
      case ak_goto:
        return dump_action(cast<goto_action>(a));
      case ak_output:
        return dump_action(cast<output_action>(a));
    }
  }

  void
  dumper::dump_action(const advance_action* a)
  {
    dump_guard g(*this, a, get_phrase_name(a));
    // FIXME: Implement me.
  }

  void
  dumper::dump_action(const copy_action* a)
  {
    dump_guard g(*this, a, get_phrase_name(a));
    // FIXME: Implement me.
  }

  void
  dumper::dump_action(const set_action* a)
  {
    dump_guard g(*this, a, get_phrase_name(a));
    // FIXME: Implement me.
  }

  void
  dumper::dump_action(const write_action* a)
  {
    dump_guard g(*this, a, get_phrase_name(a));
    // FIXME: Implement me.
  }

  void
  dumper::dump_action(const clear_action* a)
  {
    dump_guard g(*this, a, get_phrase_name(a));
  }

  void
  dumper::dump_action(const drop_action* a)
  {
    dump_guard g(*this, a, get_phrase_name(a));
  }

  void
  dumper::dump_action(const match_action* a)
  {
    dump_guard g(*this, a, get_phrase_name(a));
  }

  void
  dumper::dump_action(const goto_action* a)
  {
    dump_guard g(*this, a, get_phrase_name(a), false);
    indent();
    print_newline();
    dump_expr(a->dest);
    undent();
  }

  void
  dumper::dump_action(const output_action* a)
  {
    dump_guard g(*this, a, get_phrase_name(a));
    indent();
    print_newline();
    dump_expr(a->port);
    undent();
  }

  void
  dumper::dump_expr(const expr* e)
  {
    dump_guard g(*this, e, get_phrase_name(e));
  }

} // namespace pip

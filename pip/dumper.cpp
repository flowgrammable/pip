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
  dumper::dump_decl(const table_decl* p)
  {
    dump_guard g(*this, p, "table", false);
  }

} // namespace pip

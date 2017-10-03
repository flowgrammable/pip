#include "resolver.hpp"
#include "decl.hpp"
#include "action.hpp"
#include "expr.hpp"

#include <sstream>

namespace pip
{

  void
  resolver::resolve_decl(decl* d)
  {
    switch (get_kind(d)) {
      dk_program:
        return resolve_decl(cast<program_decl>(d));
      dk_table:
        return resolve_decl(cast<table_decl>(d));
      default:
        break;
    }
    throw std::logic_error("invalid declaration");
  }

  void
  resolver::resolve_decl(program_decl* p)
  {
    // 1st pass: generate the lookup table for devices in the program.
    for (decl* d : p->decls) {
      assert(d->id);
      decls[d->id] = d;
    }
    
    // 2nd pass: resolve references to declarations.
    for (decl* d : p->decls) 
      resolve_decl(d);
  }

  void
  resolver::resolve_decl(table_decl* t)
  {
    resolve_actions(t->prep);
    
    for (match* m : t->rules) {
      resolve_expr(m->key);
      resolve_actions(m->acts);
    }
  }

  void
  resolver::resolve_actions(action_seq& as)
  {
    for (action* a : as)
      resolve_action(a);
  }
  
  void
  resolver::resolve_action(action* a)
  {
    switch (get_kind(a)) {
      case ak_advance:
        return resolve_action(cast<advance_action>(a));
      case ak_copy:
        return resolve_action(cast<copy_action>(a));
      case ak_set:
        return resolve_action(cast<set_action>(a));
      case ak_write:
        return resolve_action(cast<write_action>(a));
      case ak_clear:
        return resolve_action(cast<clear_action>(a));
      case ak_drop:
        return resolve_action(cast<drop_action>(a));
      case ak_match:
        return resolve_action(cast<match_action>(a));
      case ak_goto:
        return resolve_action(cast<goto_action>(a));
      case ak_output:
        return resolve_action(cast<output_action>(a));
    }
  }

  void 
  resolver::resolve_action(advance_action* a)
  {
    throw std::logic_error("not implemented");
  }

  void 
  resolver::resolve_action(copy_action* a)
  {
    throw std::logic_error("not implemented");
  }

  void 
  resolver::resolve_action(set_action* a)
  {
    throw std::logic_error("not implemented");
  }

  void 
  resolver::resolve_action(write_action* a)
  {
    throw std::logic_error("not implemented");
  }

  void 
  resolver::resolve_action(clear_action* a)
  {
    // Nothing to do.
  }

  void 
  resolver::resolve_action(drop_action* a)
  {
    // Nothing to do.
  }

  void 
  resolver::resolve_action(match_action* a)
  {
    throw std::logic_error("not implemented");
  }

  void 
  resolver::resolve_action(goto_action* a)
  {
    resolve_expr(a->dest);
  }

  void 
  resolver::resolve_action(output_action* a)
  {
    resolve_expr(a->port);
  }

  void
  resolver::resolve_expr(expr* e)
  {
    switch (get_kind(e)) {
      case ek_int:
        return resolve_expr(cast<int_expr>(e));
      case ek_range:
        return resolve_expr(cast<range_expr>(e));
      case ek_wild:
        return resolve_expr(cast<wild_expr>(e));
      case ek_miss:
        return resolve_expr(cast<miss_expr>(e));
      case ek_ref:
        return resolve_expr(cast<ref_expr>(e));
      case ek_field:
        return resolve_expr(cast<field_expr>(e));
    }
  }

  void
  resolver::resolve_expr(int_expr* e)
  {
    // Nothing to do.
  }

  void
  resolver::resolve_expr(wild_expr* e)
  {
    // Nothing to do.
  }

  void
  resolver::resolve_expr(range_expr* e)
  {
    // Nothing to do.
  }

  void
  resolver::resolve_expr(ref_expr* e)
  {
    auto iter = decls.find(e->id);
    if (iter == decls.end()) {
      std::stringstream ss;
      ss << "no table named '" << e->id << '\'';
      throw lookup_error(get_location(e), ss.str());
    }
    e->ref = iter->second;
  }

  void
  resolver::resolve_expr(field_expr* e)
  {
    // FIXME: Match field expressions to internal definitions of protocol
    // information and functionality. For example, the field "eth.type"
    // should map to an object that yields the offset and length of that
    // field -- possibly also a function to actually fetch the value and
    // applying a byte order transformation.
    throw std::logic_error("not implemented");
  }

} // namespace pip

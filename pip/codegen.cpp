#include "codegen.hpp"

#include <pip/action.hpp>
#include <pip/expr.hpp>
#include <pip/decl.hpp>
#include <pip/type.hpp>

namespace pip
{

std::string
generator::generate()
{
  code << "(pip\n";
  for(auto d : ast->decls)
  {
    switch(d->kind)
    {
    case dk_table:
      generate_table_decl(static_cast<table_decl*>(d));
      break;
    default:
      throw std::runtime_error("Invalid declaration.");
    }
  }
  
  code << ")";

  return code.str();
}

void
generator::generate_table_decl(table_decl* t)
{
  code << "(table " << *t->id << " ";
  switch(t->rule)
  {
  case rk_exact:
    code << "exact\n";
    break;
  default:
    throw std::runtime_error("Invalid table rule.");
  }

  code << " ";

  generate_action_seq(t->prep);
  code << '\n';
  generate_rule_seq(t->rules);
  code << '\n';

  code << ")\n";
}

// Sequences
void
generator::generate_action_seq(action_seq& as)
{
  code << "actions(";
  for(auto a : as)
    generate_action(a);
  code << ")";
}

void
generator::generate_rule_seq(rule_seq& rs)
{
  code << "rules(\n";
  for(auto r : rs)
    generate_rule(r);
  code << ")";
}

void
generator::generate_expr_seq(expr_seq& es)
{
  for(auto e : es)
    generate_expr(e);
}


// Actions
void
generator::generate_action(action* a)
{
  switch(get_kind(a))
  {
  case ak_advance:
    generate_advance_action(static_cast<advance_action*>(a));
    break;
  case ak_copy:
    generate_copy_action(static_cast<copy_action*>(a));
    break;
  case ak_set:
    generate_set_action(static_cast<set_action*>(a));
    break;
  case ak_write:
    generate_write_action(static_cast<write_action*>(a));
    break;
  case ak_clear:
    generate_clear_action(static_cast<clear_action*>(a));
    break;
  case ak_drop:
    generate_drop_action(static_cast<drop_action*>(a));
    break;
  case ak_match:
    generate_match_action(static_cast<match_action*>(a));
    break;
  case ak_goto:
    generate_goto_action(static_cast<goto_action*>(a));
    break;
  case ak_output:
    generate_output_action(static_cast<output_action*>(a));
    break;
  }
}

void
generator::generate_advance_action(advance_action* a)
{
  code << "(advance ";
  generate_expr(a->amount);
  code << ") ";
}

static std::string
address_space_to_string(address_space as)
{
  switch(as)
  {
  case as_packet:
    return std::string("packet");
  case as_header:
    return std::string("header");
  case as_key:
    return std::string("key");
  case as_meta:
    return std::string("meta");
  }
}
  
void
generator::generate_copy_action(copy_action* a)
{
  code << "(copy ";
  generate_expr(a->src);
  generate_expr(a->dst);
  generate_expr(a->n);
  code << ") ";

}

void
generator::generate_set_action(set_action* a)
{
  code << "(set ";
  generate_expr(a->f);
  generate_expr(a->v);
  code << ") ";
}

void
generator::generate_write_action(write_action* a)
{
  code << "(write ";
  generate_action(a);
  code << ") ";
}

void
generator::generate_clear_action(clear_action* a)
{
  code << "(clear) ";
}

void
generator::generate_drop_action(drop_action* a)
{
  code << "(drop) ";
}

void
generator::generate_match_action(match_action* a)
{
  code << "(match) ";
}

void
generator::generate_goto_action(goto_action* a)
{
  code << "goto ";
  generate_expr(a->dest);
  code << ") ";
}

void
generator::generate_output_action(output_action* a)
{
  code << "output( ";
  generate_expr(a->port);
  code << ") ";
}


// Expressions
void
generator::generate_expr(expr* e)
{
  switch(get_kind(e))
  {
  case ek_int:
    return generate_int_expr(static_cast<int_expr*>(e));
  case ek_range:
    return generate_range_expr(static_cast<range_expr*>(e));
  case ek_wild:
    return generate_wild_expr(static_cast<wild_expr*>(e));
  case ek_miss:
    return generate_miss_expr(static_cast<miss_expr*>(e));
  case ek_ref:
    return generate_ref_expr(static_cast<ref_expr*>(e));
  case ek_named_field:
    return generate_named_field_expr(static_cast<named_field_expr*>(e));
  case ek_port:
    return generate_port_expr(static_cast<port_expr*>(e));
  case ek_bitfield:
    return generate_bitfield_expr(static_cast<bitfield_expr*>(e));
  }
}

void
generator::generate_int_expr(int_expr* e)
{
  code << "(int i" << static_cast<int_type*>(e->ty)->width;
  code << " " << e->val << ") ";
}

void
generator::generate_wild_expr(wild_expr* e)
{
  throw std::runtime_error("Unimplemented.");
}

void
generator::generate_range_expr(range_expr* e)
{
  code << "(range " << e->lo << " " << e->hi << ") ";
}

void
generator::generate_ref_expr(ref_expr* e)
{
  code << "(ref " << *(e->id) << ") ";
}

void
generator::generate_named_field_expr(named_field_expr* e)
{
  code << "(named_field " << *(e->field) << ") ";
}

void
generator::generate_port_expr(port_expr* e)
{
  code << "(port ";
  generate_expr(e->port_num);
  code << ") ";
}

void
generator::generate_miss_expr(miss_expr* e)
{
  code << "(miss) ";
}

void
generator::generate_bitfield_expr(bitfield_expr* e)
{
  code << "(bitfield " << address_space_to_string(e->as) << " ";
  generate_expr(e->pos);
  generate_expr(e->len);
  code << ") ";
}

// Misc
void
generator::generate_rule(rule* r)
{
  code << "(rule ";
  generate_expr(r->key);
  generate_action_seq(r->acts);
  code << ") ";
}

} // namespace pip

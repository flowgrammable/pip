#include <iostream>

#include "type_checker.hpp"

#include "decl.hpp"
#include "action.hpp"

namespace pip
{

void type_checker::check()
{
  for(auto d : prog->decls) {
    table_decl* td = static_cast<table_decl*>(d);
    check_table_decl(td);
  }
}

void type_checker::check_table_decl(table_decl* d)
{
  for(auto a : d->prep)
    check_action(a);

  for(auto r : d->rules) {
    check_expr(r->key);
    for(auto a : r->acts)
      check_action(a);
  }
}

// EXPRESSIONS

void type_checker::check_expr(expr* e)
{
  switch(e->kind)
  {
  case ek_int:
    check_int_expr(static_cast<int_expr*>(e));
    break;
  case ek_range:
    check_range_expr(static_cast<range_expr*>(e));
    break;
  case ek_wild:
    check_wild_expr(static_cast<wild_expr*>(e));
    break;
  case ek_miss:
    check_miss_expr(static_cast<miss_expr*>(e));
    break;
  case ek_ref:
    check_ref_expr(static_cast<ref_expr*>(e));
    break;
  case ek_named_field:
    check_named_field_expr(static_cast<named_field_expr*>(e));
    break;
  case ek_port:
    check_port_expr(static_cast<port_expr*>(e));
    break;
  case ek_bitfield:
    check_bitfield_expr(static_cast<bitfield_expr*>(e));
    break;
  }
    
}

void type_checker::check_int_expr(int_expr* e)
{
  if(e->ty->kind != tk_int)
    throw std::runtime_error("Integer expression of invalid type");
}

void type_checker::check_range_expr(range_expr* e)
{  
  if(e->ty->kind != tk_range)
    throw std::runtime_error("Range expression of invalid type");
}

void type_checker::check_wild_expr(wild_expr* e)
{
  if(e->ty->kind != tk_wild)
    throw std::runtime_error("Wildcard expression of invalid type");
}

void type_checker::check_miss_expr(miss_expr* e)
{
  if(e->ty->kind != tk_miss)
    throw std::runtime_error("Miss expression of invalid type");
}

void type_checker::check_ref_expr(ref_expr* e)
{
  // if(e->ty->kind != tk_ref)
    throw std::runtime_error("Reference expression of invalid type");
}

void type_checker::check_port_expr(port_expr* e)
{
  if(e->ty->kind != tk_port)
    throw std::runtime_error("Port expression of invalid type");
}

void type_checker::check_named_field_expr(named_field_expr* e)
{
  if(e->ty->kind != tk_loc)
    throw std::runtime_error("Field expression of invalid type");
}

void type_checker::check_bitfield_expr(bitfield_expr* e)
{
  if(e->ty->kind != tk_loc)
    throw std::runtime_error("Bitfield expression of invalid type");
}  

// ACTIONS
void type_checker::check_action(action* a)
{
  switch(a->kind)
  {
  case ak_advance:
    check_advance_action(static_cast<advance_action*>(a));
    break;
  case ak_copy:
    check_copy_action(static_cast<copy_action*>(a));
    break;
  case ak_set:
    check_set_action(static_cast<set_action*>(a));
    break;
  case ak_write:
    check_write_action(static_cast<write_action*>(a));
    break;
  case ak_goto:
    check_goto_action(static_cast<goto_action*>(a));
    break;
  case ak_output:
    check_output_action(static_cast<output_action*>(a));
    break;
  }
}

void type_checker::check_advance_action(advance_action* a)
{
  if(a->amount->ty->kind != tk_int)
    throw std::runtime_error("Argument in advance action must have type int.\n");
}

void type_checker::check_copy_action(copy_action* a)
{
  if(a->src->ty->kind != tk_loc)
    throw std::runtime_error("Source in copy action must be a location.\n");
  if(a->dst->ty->kind != tk_loc)
    throw std::runtime_error("Destination in copy action must be a location.\n");

  // TODO: specified is an amount to copy, is this correct?
}

void type_checker::check_set_action(set_action* a)
{
  if(a->f->ty->kind != tk_loc)
    throw std::runtime_error("Field in set action must be a location.\n");

  // TODO: allow this to be any kind of value?
  if(a->v->ty->kind != tk_int)
    throw std::runtime_error("Value in set action must have type int.\n");
}

void type_checker::check_write_action(write_action* a)
{
  check_action(a->act);
}

void type_checker::check_goto_action(goto_action* a)
{
  
}

void type_checker::check_output_action(output_action* a)
{
  if(a->port->ty->kind != tk_port)
    throw std::runtime_error("Destination in ouput action must be a port.\n");
}
  
}

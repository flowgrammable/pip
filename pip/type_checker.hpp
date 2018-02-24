#pragma once

#include <pip/syntax.hpp>

#include "type.hpp"
#include "expr.hpp"

namespace pip
{

// Performs static analysis on a translated program to ensure well-typedness.
class type_checker
{
public:
  type_checker(context& cxt, program_decl* prog)
    : cxt(cxt), prog(prog)
  { }

  void check();
private:
  void check_table_decl(table_decl* d);
  
  void check_expr(expr* e);
  void check_int_expr(int_expr* e);
  void check_range_expr(range_expr* e);
  void check_wild_expr(wild_expr* e);
  void check_ref_expr(ref_expr* e);
  void check_named_field_expr(named_field_expr* e);
  void check_port_expr(port_expr* e);
  void check_offset_expr(offset_expr* e);
  void check_miss_expr(miss_expr* e);

  void check_action(action* a);
  void check_advance_action(advance_action* a);
  void check_copy_action(copy_action* a);
  void check_set_action(set_action* a);
  void check_write_action(write_action* a);
  void check_goto_action(goto_action* a);
  void check_output_action(output_action* a);
  
private:
  context& cxt;
  program_decl* prog;
};
  
} // namespace pip

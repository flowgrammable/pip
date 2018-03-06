#pragma once

#include <pip/syntax.hpp>

#include <string>
#include <sstream>

namespace pip
{

/// Takes in a program AST as input and generates a pip program.
class generator
{
public:
  generator(program_decl* ast)
    : ast(ast)
  { }
  
  std::string generate();

private:
  // Declarations
  void generate_table_decl(table_decl* t);

  // Sequences
  void generate_action_seq(action_seq& as);
  void generate_rule_seq(rule_seq& rs);
  void generate_expr_seq(expr_seq& es);  

  // Actions
  void generate_action(action* a);
  void generate_advance_action(advance_action* a);
  void generate_copy_action(copy_action* a);
  void generate_set_action(set_action* a);
  void generate_write_action(write_action* a);
  void generate_clear_action(clear_action* a);
  void generate_drop_action(drop_action* a);
  void generate_match_action(match_action* a);
  void generate_goto_action(goto_action* a);
  void generate_output_action(output_action* a);

  // Expressions
  void generate_expr(expr* e);
  void generate_int_expr(int_expr* e);
  void generate_wild_expr(wild_expr* e);
  void generate_range_expr(range_expr* e);
  void generate_ref_expr(ref_expr* e);
  void generate_named_field_expr(named_field_expr* e);
  void generate_port_expr(port_expr* e);
  void generate_miss_expr(miss_expr* e);
  void generate_bitfield_expr(bitfield_expr* e);

  // Misc
  void generate_rule(rule* r);
  
  program_decl* ast;
  std::stringstream code;
};

}

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
  void check(table_decl* d);
  void check(expr* e);
private:
  context& cxt;
  program_decl* prog;
};
  
} // namespace pip

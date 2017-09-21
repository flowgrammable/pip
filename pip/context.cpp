#include "context.hpp"

namespace pip
{
  context::context(cc::diagnostic_manager& diags, 
                   cc::input_manager& in, 
                   cc::symbol_table& syms)
    : diags(diags), input(in), syms(syms)
  { }

  symbol*
  context::get_symbol(const char* str)
  {
    return syms.get(str);
  }

  symbol*
  context::get_symbol(const std::string& str)
  {
    return syms.get(str);
  }

} // namesapce pip

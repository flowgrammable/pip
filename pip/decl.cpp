#include "decl.hpp"
#include "dumper.hpp"

#include <iostream>

namespace pip
{

  void
  decl::dump()
  {
    dumper d(std::cerr);
    d(this);
  }

} // namespace pip

#include "expr.hpp"

namespace pip
{
  expr_kind
  get_kind(const expr* e)
  {
    return static_cast<expr_kind>(e->kind);
  }
  
  const char*
  get_phrase_name(expr_kind k)
  {
    switch (k) {
    case ek_int:
      return "int";
    case ek_range:
      return "range";
    case ek_wild:
      return "wild";
    case ek_miss:
      return "miss";
    case ek_ref:
      return "ref";
    case ek_named_field:
      return "named field";
    case ek_port:
      return "port";
    case ek_offset:
      return "offset";
    }
  }
  
  
  const char*
  get_phrase_name(const expr* e)
  {
    return get_phrase_name(get_kind(e));
  }
}

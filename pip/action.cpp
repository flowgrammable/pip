#include "action.hpp"

namespace pip
{

  action_kind 
  get_kind(const action* a)
  {
    return static_cast<action_kind>(a->kind);
  }

  const char* 
  get_phrase_name(action_kind k)
  {
    switch (k) {
      case ak_advance:
        return "advance";
      case ak_copy:
        return "copy";
      case ak_set:
        return "set";
      case ak_write:
        return "write";
      case ak_clear:
        return "clear";
      case ak_drop:
        return "drop";
      case ak_match:
        return "match";
      case ak_goto:
        return "goto";
      case ak_output:
        return "output";
    }
  }

  const char*
  get_phrase_name(const action* a)
  {
    return get_phrase_name(get_kind(a));
  }

} // namespace pip

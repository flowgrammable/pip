#pragma once

#include <pip/syntax.hpp>

namespace pip
{

enum port_kind
{
  pk_reserved,
  pk_physical,
  pk_logical,
};
  
enum reserved_ports
{
  rp_all,
  rp_controller,
  rp_table,
  rp_in_port,
  rp_any,
  rp_unset,
};

} // namespace pip

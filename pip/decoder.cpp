#include "decoder.hpp"

#include <pip/expr.hpp>
#include <pip/type.hpp>

namespace pip
{

offset_expr*
decoder::decode_named_field(named_field_expr* field) const
{
  if(*(field->field) == "eth.dst")
    return ethernet_dst_mac();
  if(*(field->field) == "eth.src")
    return ethernet_src_mac();
  if(*(field->field) == "eth.type")
    return ethernet_ethertype();

  if(*(field->field) == "ipv4.vhl")
    return ipv4_vhl();
  if(*(field->field) == "ipv4.tos")
    return ipv4_tos();
  if(*(field->field) == "ipv4.len")
    return ipv4_len();
  if(*(field->field) == "ipv4.id")
    return ipv4_id();
  if(*(field->field) == "ipv4.frag_offset")
    return ipv4_frag_offset();
  if(*(field->field) == "ipv4.ttl")
    return ipv4_ttl();
  if(*(field->field) == "ipv4.protocol")
    return ipv4_protocol();
  if(*(field->field) == "ipv4.checksum")
    return ipv4_checksum();
  if(*(field->field) == "ipv4.src")
    return ipv4_src_addr();
  if(*(field->field) == "ipv4.dst")
    return ipv4_dst_addr();

  if(*(field->field) == "tcp.src")
    return tcp_src_port();
  if(*(field->field) == "tcp.dst")
    return tcp_dst_port();
  if(*(field->field) == "tcp.seq")
    return tcp_seq();
  if(*(field->field) == "tcp.ack")
    return tcp_ack();
  if(*(field->field) == "tcp.offset")
    return tcp_offset();
  if(*(field->field) == "tcp.flags")
    return tcp_flags();
  if(*(field->field) == "tcp.window")
    return tcp_window();
  if(*(field->field) == "tcp.checksum")
    return tcp_checksum();
  if(*(field->field) == "tcp.urgent_ptr")
    return tcp_urgent_ptr();
}

offset_expr*
decoder::ethernet_dst_mac() const
{
  expr* pos = cxt.make_int_expr(new int_type(32), 0);
  expr* len = cxt.make_int_expr(new int_type(32), size_mac_addr * 8);
  return static_cast<offset_expr*>(cxt.make_offset_expr(packet_space, pos, len));
}

offset_expr*
decoder::ethernet_src_mac() const
{
  expr* pos = cxt.make_int_expr(new int_type(32), 0 + size_mac_addr * 8);
  expr* len = cxt.make_int_expr(new int_type(32), size_mac_addr * 8);
  return static_cast<offset_expr*>(cxt.make_offset_expr(packet_space, pos, len));
}

offset_expr*
decoder::ethernet_ethertype() const
{
  expr* pos = cxt.make_int_expr(new int_type(32), 2 * size_mac_addr * 8);
  expr* len = cxt.make_int_expr(new int_type(32), size_ethertype * 8);
  return static_cast<offset_expr*>(cxt.make_offset_expr(packet_space, pos, len));
}

offset_expr*
decoder::ipv4_vhl() const
{
  expr* pos = cxt.make_int_expr(new int_type(32), size_ethernet * 8);
  expr* len = cxt.make_int_expr(new int_type(32), 8);
  return static_cast<offset_expr*>(cxt.make_offset_expr(packet_space, pos, len));
}

offset_expr*
decoder::ipv4_tos() const
{
  expr* pos = cxt.make_int_expr(new int_type(32), (size_ethernet + 1) * 8);
  expr* len = cxt.make_int_expr(new int_type(32), 8);
  return static_cast<offset_expr*>(cxt.make_offset_expr(packet_space, pos, len));
}

offset_expr*
decoder::ipv4_len() const
{
  expr* pos = cxt.make_int_expr(new int_type(32), (size_ethernet + 2) * 8);
  expr* len = cxt.make_int_expr(new int_type(32), 16);
  return static_cast<offset_expr*>(cxt.make_offset_expr(packet_space, pos, len));
}

offset_expr*
decoder::ipv4_id() const
{
  expr* pos = cxt.make_int_expr(new int_type(32), (size_ethernet + 4) * 8);
  expr* len = cxt.make_int_expr(new int_type(32), 16);
  return static_cast<offset_expr*>(cxt.make_offset_expr(packet_space, pos, len));
}

offset_expr*
decoder::ipv4_frag_offset() const
{
  expr* pos = cxt.make_int_expr(new int_type(32), (size_ethernet + 4) * 8);
  expr* len = cxt.make_int_expr(new int_type(32), 16);
  return static_cast<offset_expr*>(cxt.make_offset_expr(packet_space, pos, len));
}

offset_expr*
decoder::ipv4_ttl() const
{
  expr* pos = cxt.make_int_expr(new int_type(32), (size_ethernet + 8) * 8);
  expr* len = cxt.make_int_expr(new int_type(32), 8);
  return static_cast<offset_expr*>(cxt.make_offset_expr(packet_space, pos, len));
}

offset_expr*
decoder::ipv4_protocol() const
{
  expr* pos = cxt.make_int_expr(new int_type(32), (size_ethernet + 9) * 8);
  expr* len = cxt.make_int_expr(new int_type(32), 8);
  return static_cast<offset_expr*>(cxt.make_offset_expr(packet_space, pos, len));
}

offset_expr*
decoder::ipv4_checksum() const
{
  expr* pos = cxt.make_int_expr(new int_type(32), (size_ethernet + 10) * 8);
  expr* len = cxt.make_int_expr(new int_type(32), 16);
  return static_cast<offset_expr*>(cxt.make_offset_expr(packet_space, pos, len));
}

offset_expr*
decoder::ipv4_src_addr() const
{
  expr* pos = cxt.make_int_expr(new int_type(32), (size_ethernet + 12) * 8);
  expr* len = cxt.make_int_expr(new int_type(32), 32);
  return static_cast<offset_expr*>(cxt.make_offset_expr(packet_space, pos, len));
}

offset_expr*
decoder::ipv4_dst_addr() const
{
  expr* pos = cxt.make_int_expr(new int_type(32), (size_ethernet + 16) * 8);
  expr* len = cxt.make_int_expr(new int_type(32), 32);
  return static_cast<offset_expr*>(cxt.make_offset_expr(packet_space, pos, len));
}


offset_expr*
decoder::tcp_src_port() const
{
  expr* pos = cxt.make_int_expr(new int_type(32), size_ipv4 * 8);
  expr* len = cxt.make_int_expr(new int_type(32), 16);
  return static_cast<offset_expr*>(cxt.make_offset_expr(packet_space, pos, len));
}

offset_expr*
decoder::tcp_dst_port() const
{
  expr* pos = cxt.make_int_expr(new int_type(32), size_ipv4 * 8 + 16);
  expr* len = cxt.make_int_expr(new int_type(32), 16);
  return static_cast<offset_expr*>(cxt.make_offset_expr(packet_space, pos, len));
}

offset_expr*
decoder::tcp_seq() const
{
  expr* pos = cxt.make_int_expr(new int_type(32), size_ipv4 * 8 + 32);
  expr* len = cxt.make_int_expr(new int_type(32), 32);
  return static_cast<offset_expr*>(cxt.make_offset_expr(packet_space, pos, len));
}

offset_expr*
decoder::tcp_ack() const
{
  expr* pos = cxt.make_int_expr(new int_type(32), size_ipv4 * 8 + 64);
  expr* len = cxt.make_int_expr(new int_type(32), 32);
  return static_cast<offset_expr*>(cxt.make_offset_expr(packet_space, pos, len));
}

offset_expr*
decoder::tcp_offset() const
{
  expr* pos = cxt.make_int_expr(new int_type(32), size_ipv4 + 96);
  expr* len = cxt.make_int_expr(new int_type(32), 8);
  return static_cast<offset_expr*>(cxt.make_offset_expr(packet_space, pos, len));
}

offset_expr*
decoder::tcp_flags() const
{
  expr* pos = cxt.make_int_expr(new int_type(32), size_ipv4 + 104);
  expr* len = cxt.make_int_expr(new int_type(32), 8);
  return static_cast<offset_expr*>(cxt.make_offset_expr(packet_space, pos, len));
}

offset_expr*
decoder::tcp_window() const
{
  expr* pos = cxt.make_int_expr(new int_type(32), size_ipv4 + 112);
  expr* len = cxt.make_int_expr(new int_type(32), 16);
  return static_cast<offset_expr*>(cxt.make_offset_expr(packet_space, pos, len));
}

offset_expr*
decoder::tcp_checksum() const
{
  expr* pos = cxt.make_int_expr(new int_type(32), size_ipv4 + 128);
  expr* len = cxt.make_int_expr(new int_type(32), 16);
  return static_cast<offset_expr*>(cxt.make_offset_expr(packet_space, pos, len));
}

offset_expr*
decoder::tcp_urgent_ptr() const
{
  expr* pos = cxt.make_int_expr(new int_type(32), size_ipv4 + 144);
  expr* len = cxt.make_int_expr(new int_type(32), 16);
  return static_cast<offset_expr*>(cxt.make_offset_expr(packet_space, pos, len));
}

} //namespace pip

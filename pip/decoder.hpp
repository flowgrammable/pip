#pragma once

#include <pip/syntax.hpp>
#include <pip/pcap.hpp>
#include <pip/context.hpp>
#include <pip/expr.hpp>

namespace pip
{
  class decoder
  {
  public:
    decoder(context& cxt)
      : cxt(cxt)
    { }

    bitfield_expr* decode_named_field(named_field_expr* field) const;

  private:
    bitfield_expr* ethernet_dst_mac() const;
    bitfield_expr* ethernet_src_mac() const;
    bitfield_expr* ethernet_ethertype() const;

    bitfield_expr* ipv4_vhl() const;
    bitfield_expr* ipv4_tos() const;
    bitfield_expr* ipv4_len() const;
    bitfield_expr* ipv4_id() const;
    bitfield_expr* ipv4_frag_offset() const;
    bitfield_expr* ipv4_ttl() const;
    bitfield_expr* ipv4_protocol() const;
    bitfield_expr* ipv4_checksum() const;
    bitfield_expr* ipv4_src_addr() const;
    bitfield_expr* ipv4_dst_addr() const;

    bitfield_expr* tcp_src_port() const;
    bitfield_expr* tcp_dst_port() const;
    bitfield_expr* tcp_seq() const;
    bitfield_expr* tcp_ack() const;
    bitfield_expr* tcp_offset() const;
    bitfield_expr* tcp_flags() const;
    bitfield_expr* tcp_window() const;
    bitfield_expr* tcp_checksum() const;
    bitfield_expr* tcp_urgent_ptr() const;

  private:
    context& cxt;

    const int size_ethernet = 14;
    const int size_mac_addr = 6;
    const int size_ethertype = 2;
    const int size_ipv4 = size_ethernet + 20;
  };
  
} // namespace pip

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

    offset_expr* decode_named_field(named_field_expr* field) const;

  private:
    offset_expr* ethernet_dst_mac() const;
    offset_expr* ethernet_src_mac() const;
    offset_expr* ethernet_ethertype() const;

    offset_expr* ipv4_vhl() const;
    offset_expr* ipv4_tos() const;
    offset_expr* ipv4_len() const;
    offset_expr* ipv4_id() const;
    offset_expr* ipv4_frag_offset() const;
    offset_expr* ipv4_ttl() const;
    offset_expr* ipv4_protocol() const;
    offset_expr* ipv4_checksum() const;
    offset_expr* ipv4_src_addr() const;
    offset_expr* ipv4_dst_addr() const;

    offset_expr* tcp_src_port() const;
    offset_expr* tcp_dst_port() const;
    offset_expr* tcp_seq() const;
    offset_expr* tcp_ack() const;
    offset_expr* tcp_offset() const;
    offset_expr* tcp_flags() const;
    offset_expr* tcp_window() const;
    offset_expr* tcp_checksum() const;
    offset_expr* tcp_urgent_ptr() const;

  private:
    context& cxt;

    const int size_ethernet = 14;
    const int size_mac_addr = 6;
    const int size_ethertype = 2;
    const int size_ipv4 = size_ethernet + 20;

    symbol* packet_space = new std::string("packet");
  };
  
} // namespace pip

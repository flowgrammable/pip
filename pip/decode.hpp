#pragma once

#include <pip/pcap.hpp>

#include <cstdint>
#include <cstring>
#include <iostream>
#include <vector>
#include <cassert>

#include <netinet/in.h>

#define SIZE_ETHERNET 14
#define SIZE_MAC_ADDR 6
#define SIZE_ETHERTYPE 2
#define SIZE_IPv4 SIZE_ETHERNET + 20


// TODO: refactor all of this
namespace pip
{
namespace cap
{
  /// Ethernet decoders: read only access to the fields of an ethernet frame ///
  
  inline const std::vector<unsigned char>
  ethernet_dst_mac(const unsigned char* buf)
  {
    std::vector<unsigned char> ret(SIZE_MAC_ADDR);
    std::memcpy(&ret[0], buf, SIZE_MAC_ADDR);
    return ret;
  }

  inline const std::vector<unsigned char>
  ethernet_src_mac(const unsigned char* buf)
  {
    std::vector<unsigned char> ret(SIZE_MAC_ADDR);
    std::memcpy(&ret[0], buf + SIZE_MAC_ADDR, SIZE_MAC_ADDR);
    return ret;
  }

  inline const std::vector<unsigned char>
  ethernet_ethertype(unsigned char* buf)
  {
    std::vector<unsigned char> ret(SIZE_ETHERTYPE);
    std::memcpy(&ret[0], buf + 2 * SIZE_MAC_ADDR, SIZE_ETHERTYPE);
    return ret;
  } 

  /// IPv4 decoders: read only access to the fields of an IPv4 header ///

  inline const std::vector<unsigned char>
  ipv4_vhl(unsigned char* buf)
  {
    std::vector<unsigned char> ret(1);
    std::memcpy(&ret[0], buf + SIZE_ETHERNET, 1);
    return ret;
  }

  inline const std::vector<unsigned char>
  ipv4_tos(unsigned char* buf)
  {
    std::vector<unsigned char> ret(1);
    std::memcpy(&ret[0], buf + SIZE_ETHERNET + 1, 1);
    return ret;
  }

  inline const std::vector<unsigned char>
  ipv4_len(unsigned char* buf)
  {
    std::vector<unsigned char> ret(2);
    std::memcpy(&ret[0], buf + SIZE_ETHERNET + 2, 2);
    return ret;
  }

  inline const std::vector<unsigned char>
  ipv4_id(unsigned char* buf)
  {
    std::vector<unsigned char> ret(2);
    std::memcpy(&ret[0], buf + SIZE_ETHERNET + 4, 2);
    return ret;
  }

  inline const std::vector<unsigned char>
  ipv4_frag_offset(unsigned char* buf)
  {
    std::vector<unsigned char> ret(2);
    std::memcpy(&ret[0], buf + SIZE_ETHERNET + 6, 2);
    return ret;
  }

  inline const std::vector<unsigned char>
  ipv4_ttl(unsigned char* buf)
  {
    std::vector<unsigned char> ret(1);
    std::memcpy(&ret[0], buf + SIZE_ETHERNET + 8, 1);
    return ret;
  }

  inline const std::vector<unsigned char>
  ipv4_protocol(unsigned char* buf)
  {
    std::vector<unsigned char> ret(1);
    std::memcpy(&ret[0], buf + SIZE_ETHERNET + 9, 1);
    return ret;
  }

  inline const std::vector<unsigned char>
  ipv4_checksum(unsigned char* buf)
  {
    std::vector<unsigned char> ret(2);
    std::memcpy(&ret[0], buf + SIZE_ETHERNET + 10, 2);
    return ret;
  }

  inline const std::vector<unsigned char>
  ipv4_src_addr(unsigned char* buf)
  {
    std::vector<unsigned char> ret(4);
    std::memcpy(&ret[0], buf + SIZE_ETHERNET + 12, 4);
    return ret;
  }

  inline const std::vector<unsigned char>
  ipv4_dst_addr(unsigned char* buf)
  {
    std::vector<unsigned char> ret(4);
    std::memcpy(&ret[0], buf + SIZE_ETHERNET + 16, 4);
    return ret;
  }

  /// IPv4/TCP decoders: getter functions for a TCP segment of an IPv4 packet

  inline const std::vector<unsigned char>
  tcp_src_port(unsigned char* buf)
  {
    std::vector<unsigned char> ret(2);
    std::memcpy(&ret[0], buf + SIZE_IPv4, 2);
    return ret;
  }

  inline const std::vector<unsigned char>
  tcp_dst_port(unsigned char* buf)
  {
    std::vector<unsigned char> ret(2);
    std::memcpy(&ret[0], buf + SIZE_IPv4 + 2, 2);
    return ret;
  }

  inline const std::vector<unsigned char>
  tcp_seq(unsigned char* buf)
  {
    std::vector<unsigned char> ret(4);
    std::memcpy(&ret[0], buf + SIZE_IPv4 + 4, 4);
    return ret;
  }

  inline const std::vector<unsigned char>
  tcp_ack(unsigned char* buf)
  {
    std::vector<unsigned char> ret(4);
    std::memcpy(&ret[0], buf + SIZE_IPv4 + 8, 4);
    return ret;
  }

  inline const std::vector<unsigned char>
  tcp_offset(unsigned char* buf)
  {
    std::vector<unsigned char> ret(1);
    std::memcpy(&ret[0], buf + SIZE_IPv4 + 12, 1);
    return ret;
  }

  inline const std::vector<unsigned char>
  tcp_flags(unsigned char* buf)
  {
    std::vector<unsigned char> ret(1);
    std::memcpy(&ret[0], buf + SIZE_IPv4 + 13, 1);
    return ret;
  }

  inline const std::vector<unsigned char>
  tcp_window(unsigned char* buf)
  {
    std::vector<unsigned char> ret(2);
    std::memcpy(&ret[0], buf + SIZE_IPv4 + 14, 2);
    return ret;
  }

  inline const std::vector<unsigned char>
  tcp_checksum(unsigned char* buf)
  {
    std::vector<unsigned char> ret(2);
    std::memcpy(&ret[0], buf + SIZE_IPv4 + 16, 2);
    return ret;
  }

  inline const std::vector<unsigned char>
  tcp_urgent_ptr(unsigned char* buf)
  {
    std::vector<unsigned char> ret(2);
    std::memcpy(&ret[0], buf + SIZE_IPv4 + 18, 2);
    return ret;
  }
  
  struct eth_header
  {
    eth_header(const packet& pkt)
      : pkt(pkt)
    {
      std::size_t i;
      for(i = 0; i < mac_addr_len; ++i)
	std::memcpy(&dst_mac[i], pkt.data() + i, 1);

      for(; i < mac_addr_len * 2; ++i)
	std::memcpy(&src_mac[i - mac_addr_len], pkt.data() + i, 1);

      std::memcpy(&ethertype, pkt.data() + i, 2);
    }

    void print(std::ostream& os) const
    {
      std::cout << "destination mac: ";
      for(int i = 0; i < 6; ++i)
      	os << '\t' << std::hex << (unsigned)dst_mac[i];

      os << '\n' << "source mac: ";
      for(int i = 0; i < 6; ++i)
      	os << '\t' << std::hex << (unsigned)src_mac[i];
      
      os << '\n';
      os << "ethertype: " << std::hex << ethertype << '\n';
    }      
    
    std::uint8_t dst_mac[6];
    std::uint8_t src_mac[6];
    std::uint16_t ethertype;

    const packet& pkt;

    private:
    const int mac_addr_len = 6;
  };

  struct ipv4_header
  {
    ipv4_header(const eth_header& eth)
      :eth(eth)
    {
      std::memcpy(&vhl, eth.pkt.data() + SIZE_ETHERNET, 1);      
      std::memcpy(&tos, eth.pkt.data() + SIZE_ETHERNET + 1, 1);
      std::memcpy(&len, eth.pkt.data() + SIZE_ETHERNET + 2, 2);
      std::memcpy(&id, eth.pkt.data() + SIZE_ETHERNET + 4, 2);
      std::memcpy(&frag_offset, eth.pkt.data() + SIZE_ETHERNET + 6, 2);
      std::memcpy(&ttl, eth.pkt.data() + SIZE_ETHERNET + 8, 1);
      std::memcpy(&protocol, eth.pkt.data() + SIZE_ETHERNET + 9, 1);
      std::memcpy(&checksum, eth.pkt.data() + SIZE_ETHERNET + 10, 2);
      std::memcpy(&saddr, eth.pkt.data() + SIZE_ETHERNET + 12, 4);
      std::memcpy(&daddr, eth.pkt.data() + SIZE_ETHERNET + 16, 4);

      size = (unsigned)(vhl & 0x0f);
      std::cout << "size: " << size << '\n';
      size *= 4;
      std::cout << "size*4: " << size << '\n';
    }

    void print(std::ostream& os)
    {
      // os << "size: " << (unsigned)((vhl & 0x0f)*4) << '\n';
      os << "vhl: " << (unsigned)(vhl) << '\n';
      os << "tos: " << (unsigned)tos << '\n';
      os << "len: " << ntohs(len) << '\n';
      os << "id: " << ntohs(id) << '\n';
      os << "frag_offset: " << ntohs(frag_offset) << '\n';
      os << "ttl: " << (unsigned)ttl << '\n';
      os << "proto: " << (unsigned)protocol << '\n';
      os << "checksum: " << ntohs(checksum) << '\n';
      os << "saddr: " << ntohl(saddr) << '\n';
      os << "daddr: " << ntohl(daddr) << '\n';
    }
    
    uint8_t vhl; // version << 4 | header length >> 2
    uint8_t tos;
    uint16_t len;
    uint16_t id;
    uint16_t frag_offset;
    uint8_t ttl;
    uint8_t protocol;
    uint16_t checksum;
    uint32_t saddr;
    uint32_t daddr;

    std::size_t size;
    const eth_header& eth;
  };

  struct tcp_header
  {
    tcp_header(ipv4_header& ipv4)
      : ipv4(ipv4)
    {
      const packet& pkt = ipv4.eth.pkt;
      int size_ip = SIZE_ETHERNET + 20;
      
      std::memcpy(&src_port, pkt.data() + size_ip, 2);
      std::memcpy(&dst_port, pkt.data() + size_ip + 2, 2);
      std::memcpy(&seq, pkt.data() + size_ip + 4, 4);
      std::memcpy(&ack, pkt.data() + size_ip + 8, 4);
      std::memcpy(&offset, pkt.data() + size_ip + 12, 1);
      std::memcpy(&flags, pkt.data() + size_ip + 13, 1);
      std::memcpy(&window, pkt.data() + size_ip + 14, 2);
      std::memcpy(&checksum, pkt.data() + size_ip + 16, 2);
      std::memcpy(&urgent_ptr, pkt.data() + size_ip + 18, 2);
    }

    void print(std::ostream& os)
    {
      os << "source port: " << (unsigned)src_port << '\n';
      os << "dst port: " << (unsigned)dst_port << '\n';
      os << "seq: " << ntohl(seq) << '\n';
      os << "ack: " << ntohl(ack) << '\n';
      os << "offset: " << (unsigned)offset << '\n';
      os << "flags: " << (unsigned)flags << '\n';      
      os << "window: " << ntohs(window) << '\n';
      os << "checksum: " << ntohs(checksum) << '\n';
      os << "urgent_ptr: " << ntohs(urgent_ptr) << '\n';
    }

    uint16_t src_port;
    uint16_t dst_port;
    uint32_t seq;
    uint32_t ack;
    uint8_t offset;
    uint8_t flags;
    uint16_t window;
    uint16_t checksum;
    uint16_t urgent_ptr;      
   
  private:
    ipv4_header& ipv4;
  };

  /// A structure representing a decoded packet with utilities for modificaiton.
  struct decoded_packet
  {
    virtual ~decoded_packet() {}

    virtual void set_output_port(int port) = 0;
    virtual int get_input_port() const = 0;
  };

  struct tcp_ipv4 : decoded_packet
  {
    tcp_ipv4(eth_header eth, ipv4_header ipv4, tcp_header tcp)
      :eth(eth), ipv4(ipv4), tcp(tcp)
    { }
    
    void set_output_port(int port)
    {
      // TODO: implement some kind of error checking on this cast
      tcp.dst_port = (uint16_t)port;
    }

    int get_input_port() const
    { return tcp.src_port; }
    
  private:
    eth_header eth;
    ipv4_header ipv4;
    tcp_header tcp;    
  };
} // namespace cap
} // namespace pip

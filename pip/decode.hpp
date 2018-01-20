#pragma once

#include <pip/pcap.hpp>

#include <cstdint>
#include <cstring>
#include <iostream>
#include <vector>
#include <cassert>
#include <algorithm>
#include <iterator>

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
  
  inline const std::uint64_t
  ethernet_dst_mac(const unsigned char* buf)
  {
    unsigned char copy[SIZE_MAC_ADDR];
    std::memcpy(copy, buf, SIZE_MAC_ADDR);

    std::uint64_t addr = 0;
    for(std::size_t i = 0; i < SIZE_MAC_ADDR; ++i)
      addr |= std::uint64_t(copy[i]) << (8 * (SIZE_MAC_ADDR - 1) - (i * 8));    
    
    return addr;
  }

  inline const std::uint64_t
  ethernet_src_mac(const unsigned char* buf)
  {
    unsigned char copy[SIZE_MAC_ADDR];
    std::memcpy(copy, buf + SIZE_MAC_ADDR, SIZE_MAC_ADDR);

    std::uint64_t addr = 0;
    for(std::size_t i = 0; i < SIZE_MAC_ADDR; ++i)
      addr |= std::uint64_t(copy[i]) << (8 * (SIZE_MAC_ADDR - 1) - (i * 8));

    return addr;
  }

  inline const std::uint16_t
  ethernet_ethertype(const unsigned char* buf)
  {
    std::uint16_t ethertype;
    std::memcpy(&ethertype, buf + 2 * SIZE_MAC_ADDR, SIZE_ETHERTYPE);
    
    return ntohs(ethertype);
  }

  /// Ethernet setters

  inline void
  set_ethernet_dst_mac(unsigned char* buf, std::uint64_t value)
  {
    // ensure value can be expressed in 48 bits
    assert(value <= 0xffffffffffff);

    // convert the value into a byte array
    unsigned char value_array[SIZE_MAC_ADDR];
    std::memcpy(value_array, &value, SIZE_MAC_ADDR);
    // reverse to account for endianness
    std::reverse(std::begin(value_array), std::end(value_array));
    
    std::memcpy(buf, value_array, SIZE_MAC_ADDR);
  }

  inline void
  set_ethernet_src_mac(unsigned char* buf, std::uint64_t value)
  {
    assert(value <= 0xffffffffffff);
    
    unsigned char value_array[SIZE_MAC_ADDR];
    std::memcpy(value_array, &value, SIZE_MAC_ADDR);
    std::reverse(std::begin(value_array), std::end(value_array));
    
    std::memcpy(buf + SIZE_MAC_ADDR, value_array, SIZE_MAC_ADDR);
  }

  inline void
  set_ethernet_ethertype(unsigned char* buf, std::uint16_t value)
  {
    unsigned char value_array[SIZE_ETHERTYPE];
    std::memcpy(value_array, &value, 2);
    std::reverse(std::begin(value_array), std::end(value_array));
    
    std::memcpy(buf + SIZE_MAC_ADDR * 2, value_array, SIZE_ETHERTYPE);
  }  

  /// IPv4 decoders: read only access to the fields of an IPv4 header ///

  inline const std::uint8_t
  ipv4_vhl(const unsigned char* buf)
  {
    std::uint8_t vhl;
    std::memcpy(&vhl, buf + SIZE_ETHERNET, 1);
    return vhl;
  }

  inline const std::uint8_t
  ipv4_tos(const unsigned char* buf)
  {
    std::uint8_t tos;
    std::memcpy(&tos, buf + SIZE_ETHERNET + 1, 1);
    return tos;
  }

  inline const std::uint16_t
  ipv4_len(const unsigned char* buf)
  {
    std::uint16_t len;
    std::memcpy(&len, buf + SIZE_ETHERNET + 2, 2);
    return ntohs(len);
  }

  inline const std::uint16_t
  ipv4_id(const unsigned char* buf)
  {
    std::uint16_t id;
    std::memcpy(&id, buf + SIZE_ETHERNET + 4, 2);
    return ntohs(id);
  }

  inline const std::uint16_t
  ipv4_frag_offset(const unsigned char* buf)
  {
    std::uint16_t frag_offset;
    std::memcpy(&frag_offset, buf + SIZE_ETHERNET + 6, 2);
    return frag_offset;
  }

  inline const std::uint8_t
  ipv4_ttl(const unsigned char* buf)
  {
    std::uint8_t ttl;
    std::memcpy(&ttl, buf + SIZE_ETHERNET + 8, 1);
    return ttl;
  }

  inline const std::uint8_t
  ipv4_protocol(const unsigned char* buf)
  {
    std::uint8_t protocol;
    std::memcpy(&protocol, buf + SIZE_ETHERNET + 9, 1);
    return protocol;
  }

  inline const std::uint16_t
  ipv4_checksum(const unsigned char* buf)
  {
    std::uint16_t checksum;
    std::memcpy(&checksum, buf + SIZE_ETHERNET + 10, 2);
    return checksum;
  }

  inline const std::uint32_t
  ipv4_src_addr(const unsigned char* buf)
  {
    std::uint32_t src_addr;
    std::memcpy(&src_addr, buf + SIZE_ETHERNET + 12, 4);
    return ntohl(src_addr);
  }

  inline const std::uint32_t
  ipv4_dst_addr(const unsigned char* buf)
  {
    std::uint32_t dst_addr;
    std::memcpy(&dst_addr, buf + SIZE_ETHERNET + 16, 4);
    return ntohl(dst_addr);
  }

  /// IPv4 header setters

  inline void
  set_ipv4_vhl(unsigned char* buf, std::uint8_t value)
  {
    unsigned char value_array;
    std::memcpy(&value_array, &value, 1);
    
    std::memcpy(buf + SIZE_ETHERNET, &value_array, 1);
  }

  inline void
  set_ipv4_tos(unsigned char* buf, std::uint8_t value)
  {
    unsigned char value_array;
    std::memcpy(&value_array, &value, 1);
    
    std::memcpy(buf + SIZE_ETHERNET + 1, &value_array, 1);
  }

  inline void
  set_ipv4_len(unsigned char* buf, std::uint16_t value)
  {
    unsigned char value_array[2];
    std::memcpy(value_array, &value, 2);
    std::reverse(std::begin(value_array), std::end(value_array));
    
    std::memcpy(buf + SIZE_ETHERNET + 2, value_array, 2);
  }

  inline void
  set_ipv4_id(unsigned char* buf, std::uint16_t value)
  {
    unsigned char value_array[2];
    std::memcpy(value_array, &value, 2);
    std::reverse(std::begin(value_array), std::end(value_array));
    
    std::memcpy(buf + SIZE_ETHERNET + 4, value_array, 2);
  }

  inline void
  set_ipv4_frag_offset(unsigned char* buf, std::uint16_t value)
  {
    unsigned char value_array[2];
    std::memcpy(value_array, &value, 2);
    std::reverse(std::begin(value_array), std::end(value_array));
    
    std::memcpy(buf + SIZE_ETHERNET + 6, value_array, 2);
  }

  inline void
  set_ipv4_ttl(unsigned char* buf, std::uint8_t value)
  {
    unsigned char value_array;
    std::memcpy(&value_array, &value, 1);
    
    std::memcpy(buf + SIZE_ETHERNET + 8, &value_array, 1);
  }

  inline void
  set_ipv4_protocol(unsigned char* buf, std::uint8_t value)
  {
    unsigned char value_array;
    std::memcpy(&value_array, &value, 1);
    
    std::memcpy(buf + SIZE_ETHERNET + 9, &value_array, 1);
  }

  inline void
  set_ipv4_checksum(unsigned char* buf, std::uint16_t value)
  {
    unsigned char value_array[2];
    std::memcpy(value_array, &value, 2);
    std::reverse(std::begin(value_array), std::end(value_array));
    
    std::memcpy(buf + SIZE_ETHERNET + 10, value_array, 2);
  }

  inline void
  set_ipv4_src_addr(unsigned char* buf, std::uint32_t value)
  {
    unsigned char value_array[4];
    std::memcpy(value_array, &value, 4);
    std::reverse(std::begin(value_array), std::end(value_array));
    
    std::memcpy(buf + SIZE_ETHERNET + 12, value_array, 4);
  }

  inline void
  set_ipv4_dst_addr(unsigned char* buf, std::uint32_t value)
  {
    unsigned char value_array[4];
    std::memcpy(value_array, &value, 4);
    std::reverse(std::begin(value_array), std::end(value_array));
    
    std::memcpy(buf + SIZE_ETHERNET + 16, value_array, 4);
  }

  /// IPv4/TCP decoders: getter functions for a TCP segment of an IPv4 packet

  inline const std::uint16_t
  tcp_src_port(const unsigned char* buf)
  {
    std::uint16_t port;
    std::memcpy(&port, buf + SIZE_IPv4, 2);
    return ntohs(port);
  }

  inline const std::uint16_t
  tcp_dst_port(const unsigned char* buf)
  {
    std::uint16_t port;
    std::memcpy(&port, buf + SIZE_IPv4 + 2, 2);
    return ntohs(port);
  }

  inline const std::uint32_t
  tcp_seq(const unsigned char* buf)
  {
    std::uint32_t seq;
    std::memcpy(&seq, buf + SIZE_IPv4 + 4, 4);
    return ntohl(seq);;
  }

  inline const std::uint32_t
  tcp_ack(const unsigned char* buf)
  {
    std::uint32_t ack;
    std::memcpy(&ack, buf + SIZE_IPv4 + 8, 4);
    return ntohl(ack);
  }

  inline const std::uint8_t
  tcp_offset(const unsigned char* buf)
  {
    std::uint8_t offset;
    std::memcpy(&offset, buf + SIZE_IPv4 + 12, 1);
    return offset;
  }

  inline const std::uint8_t
  tcp_flags(const unsigned char* buf)
  {
    std::uint8_t flags;
    std::memcpy(&flags, buf + SIZE_IPv4 + 13, 1);
    return flags;
  }

  inline const std::uint16_t
  tcp_window(const unsigned char* buf)
  {
    std::uint16_t window;
    std::memcpy(&window, buf + SIZE_IPv4 + 14, 2);
    return ntohs(window);
  }

  inline const std::uint16_t
  tcp_checksum(const unsigned char* buf)
  {
    std::uint16_t checksum;
    std::memcpy(&checksum, buf + SIZE_IPv4 + 16, 2);
    return ntohs(checksum);
  }

  inline const std::uint16_t
  tcp_urgent_ptr(const unsigned char* buf)
  {
    std::uint16_t ptr;
    std::memcpy(&ptr, buf + SIZE_IPv4 + 18, 2);
    return ntohs(ptr);
  }

  /// IPv4/TCP setters

  inline void
  set_tcp_src_port(unsigned char* buf, uint16_t value)
  {
    unsigned char value_array[2];
    std::memcpy(value_array, &value, 2);
    std::reverse(std::begin(value_array), std::end(value_array));
    
    std::memcpy(buf + SIZE_ETHERNET + SIZE_IPv4, value_array, 2);
  }

  inline void
  set_tcp_dst_port(unsigned char* buf, uint16_t value)
  {
    unsigned char value_array[2];
    std::memcpy(value_array, &value, 2);
    std::reverse(std::begin(value_array), std::end(value_array));
    
    std::memcpy(buf + SIZE_ETHERNET + SIZE_IPv4 + 2, value_array, 2);
  }

  inline void
  set_tcp_seq(unsigned char* buf, uint32_t value)
  {
    unsigned char value_array[4];
    std::memcpy(value_array, &value, 4);
    std::reverse(std::begin(value_array), std::end(value_array));
    
    std::memcpy(buf + SIZE_ETHERNET + SIZE_IPv4 + 4, value_array, 4);
  }

  inline void
  set_tcp_ack(unsigned char* buf, uint32_t value)
  {
    unsigned char value_array[4];
    std::memcpy(value_array, &value, 4);
    std::reverse(std::begin(value_array), std::end(value_array));
    
    std::memcpy(buf + SIZE_ETHERNET + SIZE_IPv4 + 8, value_array, 4);
  }

  inline void
  set_tcp_offset(unsigned char* buf, uint8_t value)
  {
    unsigned char value_array;
    std::memcpy(&value_array, &value, 1);
    
    std::memcpy(buf + SIZE_ETHERNET + SIZE_IPv4 + 12, &value_array, 1);
  }

  inline void
  set_tcp_flags(unsigned char* buf, uint8_t value)
  {
    unsigned char value_array;
    std::memcpy(&value_array, &value, 1);
    
    std::memcpy(buf + SIZE_ETHERNET + SIZE_IPv4 + 13, &value_array, 1);
  }

  inline void
  set_tcp_window(unsigned char* buf, uint16_t value)
  {
    unsigned char value_array[2];
    std::memcpy(value_array, &value, 2);
    std::reverse(std::begin(value_array), std::end(value_array));
    
    std::memcpy(buf + SIZE_ETHERNET + SIZE_IPv4 + 14, value_array, 2);
  }

  inline void
  set_tcp_checksum(unsigned char* buf, uint16_t value)
  {
    unsigned char value_array[2];
    std::memcpy(value_array, &value, 2);
    std::reverse(std::begin(value_array), std::end(value_array));
    
    std::memcpy(buf + SIZE_ETHERNET + SIZE_IPv4 + 16, value_array, 2);
  }

  inline void
  set_tcp_urgent_ptr(unsigned char* buf, uint16_t value)
  {
    unsigned char value_array[2];
    std::memcpy(value_array, &value, 2);
    std::reverse(std::begin(value_array), std::end(value_array));
    
    std::memcpy(buf + SIZE_ETHERNET + SIZE_IPv4 + 18, value_array, 2);
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

#include "evaluator.hpp"
#include "action.hpp"
#include "expr.hpp"
#include "decode.hpp"
#include "type.hpp"
#include "decl.hpp"
#include "dumper.hpp"
#include "context.hpp"
#include "logger.hpp"

#include <climits>
#include <sstream>
// testing only
#include <iostream>

namespace pip
{
  static cap::decoded_packet* create_modified_packet(const cap::packet& pkt)
  {
    cap::eth_header eth(pkt);

    // type == ipv4
    if(eth.ethertype = 0x800)
    {
      cap::ipv4_header ipv4(eth);

      // protocol == tcp
      if(ipv4.protocol == 6)
      {
	cap::tcp_header tcp(ipv4);
	return new cap::tcp_ipv4(eth, ipv4, tcp);
      }
	
    }

    throw std::runtime_error("Unsupported protocol.\n");
  }
  
  evaluator::evaluator(context& cxt, decl* prog, cap::packet& pkt)
    : cxt(cxt), 
      prog(prog), 
      data(pkt), 
      arrival(pkt.timestamp()),
      ingress_port(), 
      physical_port(), 
      metadata(), 
      keyreg(), 
      decode(),
      dec(cxt)
  {    
    assert(cap::ethernet_ethertype(pkt.data()) == 0x800  &&
	   "Non-ethernet frames are not supported.\n");
    assert(cap::ipv4_protocol(pkt.data()) == 0x06 &&
	   "Non-TCP packets are not supported.\n");
    
    modified_buffer = new unsigned char[pkt.size()];
    std::memcpy(modified_buffer, pkt.data(), pkt.size());
    
    modified_data = create_modified_packet(pkt);
    ingress_port = cap::tcp_src_port(pkt.data());
    
    
    // TODO: Perform static initialization. We need to evaluate all of
    // the table definitions to load them with their static rules.
    auto program = static_cast<program_decl*>(prog);
    std::vector<decl*> tables;
    for(auto declaration : program->decls)
      if(auto table = dynamic_cast<table_decl*>(declaration)) {
	for(auto r : table->rules)
	  if(auto int_key = dynamic_cast<int_expr*>(r->key))
	    table->key_table.insert(int_key->val);
	tables.push_back(table);
      }

    // TODO: Load the instructions from the first table.
    current_table = static_cast<table_decl*>(tables.front());
    for(auto a : current_table->prep)
      eval.push_back(a);
  }

  evaluator::~evaluator()
  {
    if(modified_buffer)
      delete[] modified_buffer;
  }

  const action*
  evaluator::fetch()
  {
    const action* a = eval.front();
    if(a)
      std::cout << "there is an action in eval\n";
    eval.pop_front();
    return a;
  }

  void
  evaluator::step()
  {
    // If the action queue is empty, proceed to egress processing. This
    // will execute the actions in the action list. If the action list
    // is empty, then the program is complete.
    if (eval.empty()) {
      if (actions.empty())
        return;

      // This marks the beginning of egress processing.
      eval.insert(eval.end(), actions.begin(), actions.end());
      actions.clear();
    }
    
    const action* a = fetch();
    switch (get_kind(a)) {
      case ak_advance:
        return eval_advance(cast<advance_action>(a));
      case ak_copy:
        return eval_copy(cast<copy_action>(a));
      case ak_set:
        return eval_set(cast<set_action>(a));
      case ak_write:
        return eval_write(cast<write_action>(a));
      case ak_clear:
        return eval_clear(cast<clear_action>(a));
      case ak_drop:
        return eval_drop(cast<drop_action>(a));
      case ak_match:
        return eval_match(cast<match_action>(a));
      case ak_goto:
        return eval_goto(cast<goto_action>(a));
      case ak_output:
        return eval_output(cast<output_action>(a));
        
      default:
        break;   
    }

    throw std::logic_error("invalid action");
  }

  void
  evaluator::run()
  {
    while (!done())
      step();
  }

  void
  evaluator::eval_advance(const advance_action* a)
  {
    const auto n = static_cast<int_expr*>(a->amount);
    int amount = n->val;

    decode += amount;

    std::cout << "Decode: " << decode << '\n';
  }  

  // Copy n bits from position pos in a byte array into a 64-bit integer.
  static std::uint64_t
  data_to_key_reg(std::uint8_t* bytes, std::size_t pos, std::size_t n)
  {
    const std::uint8_t* current_byte = bytes + pos / CHAR_BIT;
    std::uint64_t reg = 0;

    std::size_t offset = pos % CHAR_BIT;

    // If the copy begins between two whole bytes,
    if(offset > 0) {
      // copy the specified bits.
      reg = *(current_byte++) & (0xff >> offset);

      // If there is nothing else to copy, shift off any excess and return.
      if(n <= CHAR_BIT - offset)
	return reg >> (CHAR_BIT - offset - n);
      // Remove the copied bits from n.
      else
	n -= CHAR_BIT - offset;
    }

    // Copy all remaining whole bytes.
    while(n >= CHAR_BIT) {
      reg = (reg << CHAR_BIT) + *(current_byte++);
      n -= CHAR_BIT;
    }

    // If some fraction of a byte remains to be copied, copy it.
    if(n > 0)
      reg = (reg << n) + (*current_byte >> (CHAR_BIT - n));
	
    return reg;
  }

  // Copy n bits from one byte array to another, starting at position pos.
  static void
  bitwise_copy(std::uint8_t* dst, std::uint8_t const * src,
	       std::size_t pos, std::size_t n)
  {
    std::uint8_t* current_byte = dst + (pos / CHAR_BIT);
    std::size_t i = 0;

    // if the copy begins in between two whole bytes
    if(pos % CHAR_BIT > 0) {
      // copy just what is needed
      *current_byte |= src[i] & (0xFF >> (pos % CHAR_BIT));    

      // if there is nothing else to copy, terminate
      if(n <= CHAR_BIT - pos % CHAR_BIT)
	return;
      // otherwise, even out n to start on the next whole byte
      // (n is now a multiple of CHAR_BIT)
      else
	n -= CHAR_BIT - pos % CHAR_BIT;

      ++current_byte;
      // note that we have already accessed the first element of src
      ++i;
    }

    // copy all whole bytes that remain
    for(; n >= CHAR_BIT; n -= CHAR_BIT, ++i)
      *current_byte++ = src[i];

    // if there is a remainder of less than CHAR_BIT bits,
    // copy just that portion of the byte.
    if(n > 0) {
      *current_byte |= src[i] & (0xff << (CHAR_BIT - n));
    }  
  }
  
  void
  evaluator::eval_copy(const copy_action* a)
  {
    cxt.get_diagnostics().inform(cc::location(), "Hello world");
    auto src_loc = static_cast<offset_expr*>(a->src);
    auto dst_loc = static_cast<offset_expr*>(a->dst);
    auto n = a->n->val;

    auto src_pos = static_cast<int_expr*>(src_loc->pos);
    auto src_len = static_cast<int_expr*>(src_loc->len);
    auto dst_pos = static_cast<int_expr*>(dst_loc->pos);
    auto dst_len = static_cast<int_expr*>(dst_loc->len);

    if(*(src_loc->space) == "key")
      throw std::runtime_error("Cannot copy from a key register.\n");

    if(n > dst_len->val || n > src_len->val)
      throw std::runtime_error("Copy action overflows buffer.\n");

    if(src_len->val != dst_len->val)
      throw std::runtime_error
	("Length of copy source and destination must be equal.\n");

    if(*(dst_loc->space) == "key") {
      if(n > 64) {
	std::stringstream ss;
	ss << "Attempting to copy " << n << " bits into a register of size 64.\n";
	throw std::runtime_error(ss.str().c_str());
      }
      
      if(*(src_loc->space) == "packet") {	
	keyreg = data_to_key_reg((std::uint8_t*)data.data(), src_pos->val, src_len->val);
	std::cout << "value at packet: ";
	for(int i = 0; i < src_len->val / 8; ++i)
	  std::cout << (unsigned)*(data.data() + src_pos->val + i);
	std::cout << '\n';
      }

      else if(*(src_loc->space) == "header") {
	keyreg = data_to_key_reg((std::uint8_t*)data.data() + SIZE_ETHERNET, src_pos->val, src_len->val);
	
      }
      
      else if(*(src_loc->space) == "meta")
	throw std::runtime_error("Unimplemented.\n");

      std::cout << "Copy " << n << " bits to key register from position " << src_pos->val << ". Register value: " << keyreg << '\n';
    }
    
    else if(*(dst_loc->space) == "header") {
      if(*(src_loc->space) == "packet") {
	if((dst_len->val + dst_pos->val) > (data.size() - SIZE_ETHERNET)) {
	  std::stringstream ss;
	  ss << "Cannot write beyond buffer. Attempting to copy ";
	  ss << dst_len->val;
	  ss << " bits at position ";
	  ss << dst_pos->val;
	  ss << " of a buffer of size ";
	  ss << data.size() - SIZE_ETHERNET;
	  throw std::runtime_error(ss.str().c_str());
	}
	
	bitwise_copy(modified_buffer + SIZE_ETHERNET, (std::uint8_t*)data.data(), dst_pos->val, dst_len->val);
      }
      else if(*(src_loc->space) == "meta")
	throw std::runtime_error("Unimplemented.\n");

      std::cout << "Copy " << dst_len->val << " bits at position " << dst_pos->val << " into header. Header value: (unimplemented)\n";
      // TODO: print(header);
    }
    else if(*(dst_loc->space) == "meta")
      return;
    else if(*(dst_loc->space) == "packet") {
      if(*(src_loc->space) == "header") {
	if(dst_len->val + dst_pos->val > data.size()) {
	  std::stringstream ss;
	  ss << "Cannot write beyond buffer. Attempting to copy ";
	  ss << dst_len->val;
	  ss << " bits at position ";
	  ss << dst_pos->val;
	  ss << " of a buffer of size ";
	  ss << data.size();
	  throw std::runtime_error(ss.str().c_str());
	}
	
	bitwise_copy(modified_buffer, packet_header(data), dst_pos->val, dst_len->val);
      }

      std::cout << "Copy " << dst_len->val << " bits at position " << dst_pos->val << " into packet. packet value: (unimplemented)\n";
      // TODO: print(packet);
    }    
  }

  // Copy n bits from a uint64_t into a byte array at position pos.
  // See data_to_key_reg and bitwise_copy for further documentation.
  static void
  reg_to_buf(std::uint8_t* bytes, std::uint64_t in,
	     std::size_t pos, std::size_t n)
  {
    std::uint8_t* current_byte = bytes + pos / CHAR_BIT;
    std::size_t offset = pos % CHAR_BIT;

    if(offset > 0) {
      *current_byte++ = (std::uint8_t)in & (0xff >> offset);

      if(n <= CHAR_BIT - offset)
	return;
      else
	n -= CHAR_BIT - offset;

      in >>= offset;
    }

    while(n >= CHAR_BIT) {
      *current_byte++ = (std::uint8_t)in & 0xff;
      in >>= 8;
      n -= CHAR_BIT;
    }

    if(n > 0)
      *current_byte = (in << (CHAR_BIT - n)) + (*current_byte >> n);
  }

  void
  evaluator::eval_set(const set_action* a)
  {
    const auto loc = static_cast<offset_expr*>(a->f);
    const auto pos_expr = static_cast<int_expr*>(loc->pos);

    const auto val_expr = static_cast<int_expr*>(a->v);

    std::uint64_t value = val_expr->val;
    std::size_t val_width = static_cast<int_type*>(val_expr->ty)->width;    
    std::size_t position = pos_expr->val;

    if(val_width + position > data.size()) {
      std::stringstream ss;
      ss << "Cannot write beyond buffer. Attempting to write a value of ";
      ss << val_width;
      ss << " bits at position ";
      ss << position;
      ss << " of a buffer of size ";
      ss << data.size();
      throw std::runtime_error(ss.str().c_str());
    }

    reg_to_buf(modified_buffer, value, position, val_width);

    std::cout << "Set " << val_width << " bits of packet to " << value << ". Value of packet: (unimplemented).\n";
  }

  void
  evaluator::eval_write(const write_action* a)
  {
    actions.push_back(a);
    std::cout << "Write action:\n";
    dumper d(std::cout);
    d(a);
  }

  void
  evaluator::eval_clear(const clear_action* a)
  {
    eval.clear();
    std::cout << "Clear.\n";
  }

  void
  evaluator::eval_drop(const drop_action* a)
  {
    eval.clear();
    actions.clear();
    std::cout << "Drop.\n";
  }

  void
  evaluator::eval_match(const match_action* a)
  {
    // If one of the rules matches the key register, then evaluate
    // that rule's action list.

    std::cout << "keyreg: " << keyreg << '\n';
    std::cout << "keyreg ntohs: " << ntohs(keyreg) << '\n';
    
    for(auto r : current_table->rules)
    {
      auto key = r->key;

      if(get_kind(key) == ek_miss) {
      	std::cout << "packet missed.\n";
      	eval.insert(eval.end(), r->acts.begin(), r->acts.end());
      	break;
      }

      if(current_table->key_table.find(keyreg) != current_table->key_table.end()) {
	std::cout << keyreg << " was matched in table.\n";
	eval.insert(eval.end(), r->acts.begin(), r->acts.end());
	break;
      }
    }
  }


  void
  evaluator::eval_goto(const goto_action* a)
  {
    // goto is a terminator action; eval must be empty at this point for this
    // program to be well formed.
    assert(eval.empty());
    
    ref_expr* dst = static_cast<ref_expr*>(a->dest);
    current_table = static_cast<table_decl*>(dst->ref);

    for(auto a : current_table->prep)
      eval.push_back(a);

    std::cout << "Goto table: " << *(dst->id) << '\n';
  }

  void
  evaluator::eval_output(const output_action* a)
  {
    const port_expr* port = static_cast<port_expr*>(a->port);

    int port_num;
    if(port->port_kind == port_expr::pk_int)
      port_num = static_cast<int_expr*>(port->port_num)->val;
    else if(port->port_kind == port_expr::pk_loc) {
      auto loc = static_cast<offset_expr*>(port->port_num);
      auto pos = static_cast<int_expr*>(loc->pos);
      auto len = static_cast<int_expr*>(loc->len);
      port_num = data_to_key_reg((std::uint8_t*)data.data(), pos->val, len->val);
    }

    egress_port = port_num;
    std::cout << "Output to port: " << egress_port << '\n';
  }

  std::uint64_t
  evaluator::eval_field_expr(const field_expr* e)
  {
    const std::string field = *e->field;
    if(field == "eth.dst")
      return cap::ethernet_dst_mac(data.data());
    else if(field == "eth.src")
      return cap::ethernet_src_mac(data.data());
    else if(field == "eth.type")
      return cap::ethernet_ethertype(data.data());
    else if(field == "ipv4.vhl")
      return cap::ipv4_vhl(data.data());
    else if(field == "ipv4.tos")
      return cap::ipv4_tos(data.data());
    else if(field == "ipv4.len")
      return cap::ipv4_len(data.data());
    else if(field == "ipv4.id")
      return cap::ipv4_id(data.data());
    else if(field == "ipv4.frag")
      return cap::ipv4_frag_offset(data.data());
    else if(field == "ipv4.ttl")
      return cap::ipv4_ttl(data.data());
    else if(field == "ipv4.protocol")
      return cap::ipv4_protocol(data.data());
    else if(field == "ipv4.checksum")
      return cap::ipv4_checksum(data.data());
    else if(field == "ipv4.src")
      return cap::ipv4_src_addr(data.data());
    else if(field == "ipv4.dst")
      return cap::ipv4_dst_addr(data.data());
    else if(field == "tcp.src")
      return cap::tcp_src_port(data.data());
    else if(field == "tcp.dst")
      return cap::tcp_dst_port(data.data());
    else if(field == "tcp.seq")
      return cap::tcp_seq(data.data());
    else if(field == "tcp.ack")
      return cap::tcp_ack(data.data());
    else if(field == "tcp.offset")
      return cap::tcp_offset(data.data());
    else if(field == "tcp.flags")
      return cap::tcp_flags(data.data());
    else if(field == "tcp.window")
      return cap::tcp_window(data.data());
    else if(field == "tcp.urgent_ptr")
      return cap::tcp_urgent_ptr(data.data());
  }



} // namespace pip

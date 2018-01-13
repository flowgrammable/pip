#include <pip/context.hpp>
#include <pip/decl.hpp>
#include <pip/translator.hpp>
#include <pip/resolver.hpp>
#include <pip/evaluator.hpp>
#include <pip/type_checker.hpp>
#include <pip/pcap.hpp>
#include <pip/decode.hpp>

#include <sexpr/syntax.hpp>
#include <sexpr/context.hpp>
#include <sexpr/parser.hpp>

#include <cc/input.hpp>
#include <cc/output.hpp>
#include <cc/diagnostics.hpp>

#include <iostream>

#include <cstring>
#include <cstdint>
#include <netinet/in.h>
#include <climits>
#include <bitset>


template <typename R>
static constexpr R bitmask(unsigned int const onecount)
{
    return static_cast<R>(-(onecount != 0))
        & (static_cast<R>(-1) >> ((sizeof(R) * CHAR_BIT) - onecount));
}

template<typename R>
static constexpr R bitfieldmask(unsigned int const a, unsigned int const b)
{
  return ((static_cast<R>(-1) >> (((sizeof(R) * CHAR_BIT) - 1) - (b)))
	  & ~((1 << (a)) - 1));
	 
}

int
main(int argc, char* argv[])
{
  /// The file use for writing errors.
  cc::output_device error = std::cerr;

  /// Manages error diagnostics.
  cc::diagnostic_manager diags;

  /// Manages in the input source.
  cc::input_manager inputs;

  /// A set of unique symbols.
  cc::symbol_table syms;

  /// The program translation context.
  pip::context cxt(diags, inputs, syms);

  if (argc < 3) {
    std::cerr << "usage: pip <pip-program> <pcap-file>\n";
    return 1;
  }

  // Parse the input file as a pip program. There are a number of
  // stages of translation:
  pip::decl* prog;
  try {
    // Stage 1. Accept the input file.
    const cc::file& input = inputs.add_file(argv[1]);

    // Stage 2: Parse the program as an uninterpreted s-expression.
    sexpr::context sexpr(diags, inputs, syms);
    sexpr::parser parse(sexpr, input);
    sexpr::expr* e = parse();
    e->dump();

    // Stage 3: Match the s-expression into a pip program.
    pip::translator trans(cxt);
    prog = trans(e);
    prog->dump();

    std::cout << "End translation\n";

    // Stage 4: Name lookup. Match identifiers to declarations.
    pip::resolver resolve(cxt);
    resolve(prog);

    // Stage 5: Type checking. Assign a type to each expression.
    pip::type_checker types(cxt, static_cast<pip::program_decl*>(prog));
    types.check();

    // Stage K: Other static analysis?

    // ...

    // Stage K + N: Evaluate the program.
    //
    // TODO: Build a packet stream evaluator that encapsulates this 
    // functionality.
    int partial = 0;
    pip::cap::file in(argv[2]);
    pip::cap::packet pkt;
    while (in.get(pkt)) {

      std::cout << "PKT:\n";
      uint8_t* vec = new uint8_t[pkt.size()];
      for(int i = 0; i < pkt.size(); ++i)
      {
      	uint8_t n;
      	std::memcpy(&n, pkt.data() + i, 1);
	vec[i] = n;
	
      	std::cout << std::hex << (unsigned)vec[i] << '\t';

      	if(i > 1 && i % 8 == 0)
      	  std::cout << '\n';
      }
      // pip::cap::eth_header eth(pkt);
      // const pip::cap::eth_header* eth;
      // eth = (pip::cap::eth_header*)(pkt.data());
      pip::cap::eth_header eth(pkt);
      // eth.print();
      std::cout << "\nETH:\n";
      eth.print(std::cout);
      std::cout << "\nIP:\n";
      pip::cap::ipv4_header ipv4(eth);
      ipv4.print(std::cout);

      std::cout << "\nTCP:\n";
      pip::cap::tcp_header tcp(ipv4);
      tcp.print(std::cout);

      // int bitwidth = 7;
      // uint64_t field = 0;
      // uint64_t value = 94;
      // int field_width = 16;
      // int enjamb = (field_width - (field_width % 8)) / 8;
      // uint64_t mask = bitmask<uint32_t>(bitwidth);

      // // We can enjamb a value into the beginning of an integer
      // // by creating a mask and ORing 
      // if(bitwidth < sizeof(value) * 8)
      // {
      // 	mask << ((sizeof(value) * 8) - bitwidth);
      // 	field = (field & ~mask) | (value << (sizeof(value) * 8) - bitwidth);
      // 	uint8_t* exploded_field = new uint8_t[8];

      // 	std::memcpy(exploded_field, &field, 8);

      // 	std::bitset<64> x(field);
      // 	std::cout << "Enjambed number: " << x << '\n';

      // 	std::cout << "Enjambed array: ";
      // 	for(int i = 0; i < enjamb; ++i)
      // 	{
      // 	  std::cout << exploded_field[i] << " ";
      // 	}
      // 	std::cout << '\n';
      // }

      
      // std::cout << "\nNEW BUF\n";
      // for(int i = 0; i < pkt.size(); ++i)
      // {
      // 	std::cout << std::hex << (unsigned)vec[i] << '\t';

      // 	if(i > 1 && i % 8 == 0)
      // 	  std::cout << '\n';
      // }      
      delete[] vec;
      
      // for(int i = 0; i < 6; ++i)
      // 	std::cout << std::hex << (unsigned)eth->dst_mac[i] << '\t';

      // std::cout << '\n';
      // for(int i = 0; i < 6; ++i)
      // 	std::cout << std::hex << (unsigned)eth->src_mac[i] << '\t';
      
      // std::cout << '\n';
      //   std::cout << std::hex << (unsigned)eth->ethertype << '\t';
					      
      // for(int i = 0; i < sizeof(eth->payload); ++i) {
      // 	if(i > 1 && i % 8 == 0)
      // 	  std::cout << '\n';
      // 	std::cout << std::hex << (unsigned)eth->payload[i] << '\t';	
      // }
      
      // Ignore incomplete packets.
      if(!pkt.is_complete()) {
        ++partial;
        continue;
      }

      pip::evaluator eval(cxt, nullptr, pkt);

      // TODO: This is where we could turn this into a debugger. Simply
      // allowing the user to invoke the step command would enable them
      // to step through the execution of the packet in the pipeline.
      // eval.run();
    }

    std::cout << "partial packets: " << partial << '\n';
  }
  catch (cc::diagnosable_error& err) {
    diags.emit(err);
  }

  // Emit any errors.
  if (diags.error_count())
    print(diags.get_diagnostics(), inputs, error);  
}

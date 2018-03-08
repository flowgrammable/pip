#include <pip/context.hpp>
#include <pip/decl.hpp>
#include <pip/translator.hpp>
#include <pip/resolver.hpp>
#include <pip/evaluator.hpp>
#include <pip/type_checker.hpp>
#include <pip/pcap.hpp>
#include <pip/decode.hpp>
#include <pip/codegen.hpp>

#include <sexpr/syntax.hpp>
#include <sexpr/context.hpp>
#include <sexpr/parser.hpp>

#include <cc/input.hpp>
#include <cc/output.hpp>
#include <cc/diagnostics.hpp>

#include <iostream>
#include <fstream>

#include <cstring>
#include <cstdint>
#include <netinet/in.h>
#include <climits>
#include <algorithm>


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

    // If the amount of physical is not defined, it is the maximum 32-bit uinteger.
    std::uint32_t physical_ports = (std::uint32_t)(~0);
    std::vector<std::string> arguments;
    arguments.assign(argv, argv + argc);

    // Otherwise, restrict the set if it is in range.
    auto port_arg_it = std::find(arguments.begin(), arguments.end(), "-p");
    if(port_arg_it == arguments.end())
      port_arg_it = std::find(arguments.begin(), arguments.end(), "--ports");
    
    if(port_arg_it != arguments.end()) {
      std::string amount_string = *(port_arg_it + 1);
      std::size_t size;
      int amount = std::stoi(amount_string, &size);
      if(amount_string.size() != size)
	throw std::runtime_error("Invalid amount of physical ports. Usage: -p <uint32> or --ports <uint32>.");
      if(amount < 1 || amount > physical_ports)
	throw std::runtime_error("Amount of physical ports must be a 32-bit unsigned integer.");
      physical_ports = amount;
    }

    // Stage 2: Parse the program as an uninterpreted s-expression.
    sexpr::context sexpr(diags, inputs, syms);
    sexpr::parser parse(sexpr, input);
    sexpr::expr* e = parse();
    e->dump();

    // Stage 3: Match the s-expression into a pip program.
    pip::translator trans(cxt);
    prog = trans(e);
    prog->dump();
    auto program = static_cast<pip::program_decl*>(prog);

    std::cout << "End translation\n";
    std::ofstream outfile("out.pip");
    pip::generator gen(dynamic_cast<pip::program_decl*>(prog));
    outfile << gen.generate();
    outfile.close();

    // Stage 4: Name lookup. Match identifiers to declarations.
    pip::resolver resolve(cxt);
    resolve(prog);

    // // Stage 5: Type checking. Assign a type to each expression.
    // pip::type_checker types(cxt, program);
    // types.check();

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

      for(auto d : program->decls) {
	if(auto t = dynamic_cast<pip::table_decl*>(d)) {
	  for(auto r : t->rules)
	    if(auto i = dynamic_cast<pip::int_expr*>(r->key))
	      std::cout << "rule key: " << i->val << '\n';
	}
      }
      
      pip::evaluator eval(cxt, program, pkt, physical_ports);
      eval.run();

    }
      // TODO: This is where we could turn this into a debugger. Simply
      // allowing the user to invoke the step command would enable them
      // to step through the execution of the packet in the pipeline.
      // eval.run();

    std::cout << "partial packets: " << partial << '\n';
  }
  catch (cc::diagnosable_error& err) {
    diags.emit(err);
  }

  // Emit any errors.
  if (diags.error_count())
    print(diags.get_diagnostics(), inputs, error);  
}

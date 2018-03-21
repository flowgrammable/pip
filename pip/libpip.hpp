#pragma once

#include <pip/context.hpp>
#include <pip/decl.hpp>
#include <pip/translator.hpp>
#include <pip/resolver.hpp>
#include <pip/evaluator.hpp>
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
#include <cstdint>

namespace pip
{

class pip_init
{
public:
  inline pip_init(int argc, char* argv[])
    : cxt(context(diags, inputs, syms))
  {
    if (argc < 3) {
      throw std::logic_error("usage: pip <pip-program> <pcap-file>");
    }

    try {
      // Stage 1. Accept the input file.
      const cc::file& input = inputs.add_file(argv[1]);

      // If the amount of physical is not defined, it is the maximum 32-bit uinteger.
      physical_ports = (std::uint32_t)(~(std::uint32_t(0)));
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
	  throw std::logic_error("Invalid amount of physical ports. Usage: -p <uint32> or --ports <uint32>.");
      
	if(amount < 1 || amount > physical_ports)
	  throw std::logic_error("Amount of physical ports must be a 32-bit unsigned integer.");
      
	physical_ports = amount;
      }

      // Stage 2: Parse the program as an uninterpreted s-expression.
      sexpr::context sexpr(diags, inputs, syms);
      sexpr::parser parse(sexpr, input);
      sexpr::expr* e = parse();

      // Stage 3: Match the s-expression into a pip program.
      translator trans(cxt);
      prog = trans(e);

      // Stage 4: Name lookup. Match identifiers to declarations.
      resolver resolve(cxt);
      resolve(prog);
    }

    catch(cc::diagnosable_error& err) {
      diags.emit(err);
    }
  }

  inline decl* get_program() const { return prog; }
  inline context& get_context() { return cxt; }
  
  inline evaluator build_evaluator(cap::packet& pkt)
  {
    return evaluator(cxt, static_cast<program_decl*>(prog), pkt, physical_ports);
  }


private:
  /// The file use for writing errors.
  cc::output_device error = std::cerr;

  /// Manages error diagnostics.
  cc::diagnostic_manager diags;

  /// Manages in the input source.
  cc::input_manager inputs;

  /// A set of unique symbols.
  cc::symbol_table syms;

  /// The program translation context.
  context cxt;

  /// The AST of the program.
  decl* prog;

  std::uint32_t physical_ports;
};

} //namespace pip

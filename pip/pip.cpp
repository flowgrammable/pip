#include <pip/context.hpp>
#include <pip/evaluator.hpp>

#include <cc/input.hpp>
#include <cc/output.hpp>
#include <cc/diagnostics.hpp>

#include <iostream>

using namespace pip;

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
  context cxt(diags, inputs, syms);

  if (argc < 3) {
    std::cerr << "usage: pip <pip-program> <pcap-file>\n";
    return 1;
  }

  // TODO: Load the program. This requires opening a pip file, translating
  // it, and performing some set of lowering and optimizing passes.
  
  // Process packets.
  int partial = 0;
  cap::file in(argv[2]);
  cap::packet pkt;
  while (in.get(pkt)) {
    // Ignore incomplete packets.
    if(!pkt.is_complete()) {
      ++partial;
      continue;
    }

    evaluator eval(cxt, nullptr, pkt);

    // TODO: This is where we could turn this into a debugger. Simply
    // allowing the user to invoke the step command would enable them
    // to step through the execution of the packet in the pipeline.
    eval.run();
  }

  std::cout << "partial packets: " << partial << '\n';

}

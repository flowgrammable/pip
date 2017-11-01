#include <pip/context.hpp>
#include <pip/decl.hpp>
#include <pip/translator.hpp>
#include <pip/resolver.hpp>
#include <pip/evaluator.hpp>
#include <pip/pcap.hpp>

#include <sexpr/syntax.hpp>
#include <sexpr/context.hpp>
#include <sexpr/parser.hpp>

#include <cc/input.hpp>
#include <cc/output.hpp>
#include <cc/diagnostics.hpp>

#include <iostream>

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

    // Stage 4: Name lookup. Match identifiers to declarations.
    pip::resolver resolve(cxt);
    resolve(prog);

    // Stage 5: Type checking. Assign a type to each expression.

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
      // Ignore incomplete packets.
      if(!pkt.is_complete()) {
        ++partial;
        continue;
      }

      pip::evaluator eval(cxt, nullptr, pkt);

      // TODO: This is where we could turn this into a debugger. Simply
      // allowing the user to invoke the step command would enable them
      // to step through the execution of the packet in the pipeline.
      eval.run();
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

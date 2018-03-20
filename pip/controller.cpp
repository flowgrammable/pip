#include <pip/syntax.hpp>
#include <pip/translator.hpp>
#include <pip/evaluator.hpp>
#include <pip/pcap.hpp>
#include <pip/resolver.hpp>

#include <sexpr/syntax.hpp>
#include <sexpr/context.hpp>
#include <sexpr/parser.hpp>

#include <cc/input.hpp>
#include <cc/output.hpp>
#include <cc/diagnostics.hpp>

#include <iostream>

controller(pip::evaluator& eval);

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
    auto program = static_cast<pip::program_decl*>(prog);

    // Stage 4: Name lookup. Match identifiers to declarations.
    pip::resolver resolve(cxt);
    resolve(prog);

    int partial = 0;
    pip::cap::file in(argv[2]);
    pip::cap::packet pkt;
    while (in.get(pkt)) {
      for(auto d : program->decls) {
	if(get_kind(d) == dk_table) {
	  auto t = static_cast<table_decl*>(d);
	  for(auto r : t->rules) {
	    if(get_kind(r->key) == ek_int) {
	      auto i = static_cast<pip::int_expr*>(r->key);
	      std::cout << "rule key: " << i->val << '\n';
	    }
	  }
	}
      }
    }
      
    pip::evaluator eval(cxt, program, pkt);
    eval.run();

    if(eval.get_egress_port() == 69);

  }

  std::cout << "partial packets: " << partial << '\n';

  catch (cc::diagnosable_error& err) {
    diags.emit(err);
  }

  // Emit any errors.
  if (diags.error_count())
    print(diags.get_diagnostics(), inputs, error);

  return 0;
}

controller(pip::evaluator& eval)
{
}

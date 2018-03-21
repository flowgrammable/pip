#include <pip/libpip.hpp>
#include <pip/pcap.hpp>

int
main(int argc, char* argv[])
{
  pip::pip_init init(argc, argv);

  pip::cap::file in(argv[2]);
  pip::cap::packet pkt;
  
  while (in.get(pkt)) {
    auto eval = init.build_evaluator(pkt);
    eval.run();
  }  
}


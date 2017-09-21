#include <pip/pcap.hpp>

#include <iostream>

using namespace pip;

int
main(int argc, char* argv[])
{
  if (argc < 2) {
    std::cerr << "usage: pip <pcap-file>\n";
    return 1;
  }

  cap::file in(argv[1]);
  std::cout << "HERE: " << (bool)in << '\n';
  cap::packet pkt;
  int n = 0;
  while (in.get(pkt)) {
    std::cout << n << '\n';
    ++n;
  }

  return 0;
}

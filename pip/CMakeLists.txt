add_library(libpip
  type.cpp
  expr.cpp
  action.cpp
  context.cpp
  sexpr.cpp
  evaluator.cpp
  pcap.cpp)

add_executable(pip pip.cpp)
target_link_libraries(pip
  libpip 
  ${CC_LIBRARY} 
  ${SEXPR_LIBRARY} 
  ${PCAP_LIBRARY})

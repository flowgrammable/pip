add_library(libpip
  type.cpp
  expr.cpp
  decl.cpp
  type.cpp
  action.cpp
  context.cpp
  dumper.cpp
  translator.cpp
  type_checker.cpp
  resolver.cpp
  evaluator.cpp
  pcap.cpp
  decoder.cpp
  codegen.cpp
  libpip.cpp)

add_executable(pip pip.cpp)
target_link_libraries(pip
  libpip 
  ${CC_LIBRARY} 
  ${SEXPR_LIBRARY} 
  ${PCAP_LIBRARY})

# an example controller program
add_executable(wire wire.cpp)
target_link_libraries(wire
  libpip 
  ${CC_LIBRARY} 
  ${SEXPR_LIBRARY} 
  ${PCAP_LIBRARY})
set_target_properties(wire
  PROPERTIES
  ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/wire"
  LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/wire"
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/wire")

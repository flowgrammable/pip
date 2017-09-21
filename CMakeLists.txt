
cmake_minimum_required(VERSION 3.4)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

project(pip C CXX)

set(CMAKE_CXX_FLAGS "-std=c++14")

find_package(CC)
find_package(Sexpr)
find_package(PCAP)

# Put these directories in the header path.
include_directories(
  ${CMAKE_SOURCE_DIR} 
  ${CC_INCLUDE_DIR}
  ${SEXPR_INCLUDE_DIR}
  ${PCAP_INCLUDE_DIR}
)

# Put these directories in the linker path.
link_directories(
  ${CC_LIBRARY_DIR}
  ${SEXPR_LIBRARY_DIR}
)

add_subdirectory(pip)

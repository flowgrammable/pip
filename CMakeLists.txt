
cmake_minimum_required(VERSION 3.4)

project(pip CXX)

set(CMAKE_CXX_FLAGS "-std=c++14")

# FIXME:
# Find PCAP stuff.

# Find pre-installed cc components.
find_library(CC_LIBRARY cc)
find_path(CC_INCLUDE_DIR cc/node.hpp)
get_filename_component(CC_LIBRARY_DIR ${CC_LIBRARY} DIRECTORY CACHE)

# Find the pre-installed sexpr components.
find_library(SEXPR_LIBRARY sexpr)
find_path(SEXPR_INCLUDE_DIR sexpr/syntax.hpp)
get_filename_component(SEXPR_LIBRARY_DIR ${SEXPR_LIBRARY} DIRECTORY CACHE)

# Put these directories in the header path.
include_directories(
  ${CMAKE_SOURCE_DIR} 
  ${CC_INCLUDE_DIR}
  ${SEXPR_INCLUDE_DIR}
)

# Put these directories in the linker path.
link_directories(
  ${CC_LIBRARY_DIR}
  ${SEXPR_LIBRARY_DIR}
)

add_subdirectory(pip)

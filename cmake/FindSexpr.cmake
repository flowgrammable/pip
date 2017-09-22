# - Try to find libexpr include dirs and libraries
#
# Usage of this module as follows:
#
#     find_package(Sexpr)
#
# Variables defined by this module:
#
#  SEXPR_FOUND                System has libsexpr, include and library dirs found
#  SEXPR_INCLUDE_DIR          Path to the libsexpr include directory.
#  SEXPR_LIBRARY_DIR          Path to the libsexpr library directory.
#  SEXPR_LIBRARY              The libsexpr library.


find_path(SEXPR_INCLUDE_DIR 
  cc/node.hpp
)

find_library(SEXPR_LIBRARY 
  cc
)

get_filename_component(SEXPR_LIBRARY_DIR 
  ${SEXPR_LIBRARY} DIRECTORY CACHE
)


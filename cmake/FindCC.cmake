# - Try to find libcc include dirs and libraries
#
# Usage of this module as follows:
#
#     find_package(CC)
#
# Variables defined by this module:
#
#  CC_FOUND                System has libcc, include and library dirs found
#  CC_INCLUDE_DIR          Path to the libcc include directory.
#  CC_LIBRARY_DIR          Path to the libcc library directory.
#  CC_LIBRARY              The libcc library.


find_path(CC_INCLUDE_DIR 
  cc/node.hpp
)

find_library(CC_LIBRARY 
  cc
)

get_filename_component(CC_LIBRARY_DIR 
  ${CC_LIBRARY} DIRECTORY CACHE
)


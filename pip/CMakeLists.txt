add_library(pip
  type.cpp
  expr.cpp
  action.cpp
  sexpr.cpp)

add_executable(pip-run pip.cpp)
target_link_libraries(pip-run pip ${CC_LIBRARY} ${SEXPR_LIBRARY})

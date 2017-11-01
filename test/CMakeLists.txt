
set(compiler ${CMAKE_BINARY_DIR}/pip/pip-compile)

add_test(test1 ${compiler} 1.pip)

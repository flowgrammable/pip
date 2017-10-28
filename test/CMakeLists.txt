# Create the testing file and list of tests 
create_test_sourcelist(Tests 
  main.cpp 
  1.pip
  2.pip 
  )

# Include pip directory 
include_directories("${CMAKE_SOURCE_DIR}/pip")

# Add the executable 
add_executable(main.cpp ${Tests})

# Remove the test driver source file 
set(tests_to_run ${Tests})
remove(tests_to_run test.cpp)

# Add all the add_test for each test 
foreach(test ${tests_to_run})
  get_filename_component(TName ${test} NAME_WE)
  add_test(${TName} test.cpp ${TName})
endforeach()



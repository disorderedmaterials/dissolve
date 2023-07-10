# Add Example Test Function
function(dissolve_example_test test_name input_file)
  message(STATUS "... Example test '${test_name}' in directory '${CMAKE_CURRENT_LIST_DIR}'")
  set(test_target_executable ${CMAKE_BINARY_DIR}/bin/${target_name})
  add_test(
    NAME ${CMAKE_CURRENT_LIST_DIR}-${test_name}
    COMMAND ${test_target_executable} ${input_file} -i -w ${input_file}.rewrite --reload-after-write
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
  )
  set_property(TEST ${CMAKE_CURRENT_LIST_DIR}-${test_name} PROPERTY LABELS ${CMAKE_CURRENT_LIST_DIR})
endfunction()

# List system test subdirectories
message(STATUS "Looking for example tests in ${CMAKE_CURRENT_SOURCE_DIR}...")
file(
  GLOB children
  RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
  "*"
)
set(EXAMPLE_TEST_DIRS "")
foreach(child ${children})
  if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${child} AND EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${child}/CMakeLists.txt)
    list(APPEND EXAMPLE_TEST_DIRS ${child})
  endif()
endforeach()

foreach(subdir ${EXAMPLE_TEST_DIRS})
  add_subdirectory(${subdir})
endforeach()

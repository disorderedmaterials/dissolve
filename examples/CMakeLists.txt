# List example subdirectories
message(STATUS "Looking for example tests in ${CMAKE_CURRENT_SOURCE_DIR}...")
file(
  GLOB children
  RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
  "*"
)
set(EXAMPLE_TEST_DIRS "")
foreach(child ${children})
  if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${child} AND EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${child}/input.txt)
    message(STATUS "... Example test '${child}' in directory '${CMAKE_CURRENT_LIST_DIR}/${child}'")
    set(test_target_executable ${CMAKE_BINARY_DIR}/bin/${target_name})
    add_test(
      NAME ${CMAKE_CURRENT_LIST_DIR}-${child}
      COMMAND ${test_target_executable} input.txt -i -w input.txt.rewrite --reload-after-write
      WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/${child}
    )
    set_property(TEST ${CMAKE_CURRENT_LIST_DIR}-${child} PROPERTY LABELS ${CMAKE_CURRENT_LIST_DIR}/${child})
  endif()
endforeach()

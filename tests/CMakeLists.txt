# Testing Code
enable_testing()

function(dissolve_system_test directory test_name input_file count)
  message(STATUS "... System test '${test_name}' in directory '${directory}'")
  if(PARALLEL)
    foreach(nproc 1 2)
      set(test_target_executable ${CMAKE_BINARY_DIR}/bin/${target_name})
      add_test(
        NAME ${directory}-${test_name}-${nproc}
        COMMAND ${MPIEXEC_EXECUTABLE} ${MPIEXEC_NUMPROC_FLAG} ${nproc} ${test_target_executable} -n ${count} -x ${input_file} ${ARGN}
        WORKING_DIRECTORY ../tests/${directory}
      )
      set_property(TEST ${directory}-${test_name}-${nproc} PROPERTY LABELS ${directory})
      set_property(TEST ${directory}-${test_name}-${nproc} PROPERTY PROCESSORS ${nproc})
    endforeach()
  else()
    set(test_target_executable ${CMAKE_BINARY_DIR}/bin/${target_name})
    add_test(
      NAME ${directory}-${test_name}
      COMMAND ${test_target_executable} -n ${count} -x ${input_file} ${ARGN}
      WORKING_DIRECTORY ../tests/${directory}
    )
    set_property(TEST ${directory}-${test_name} PROPERTY LABELS ${directory})
  endif()
endfunction()

# System test subdirectories
message(STATUS "Looking for system tests in ${CMAKE_CURRENT_SOURCE_DIR}...")
file(
  GLOB children
  RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
  "*"
)
set(SYSTEM_TEST_DIRS "")
foreach(child ${children})
  if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${child} AND EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${child}/CMakeLists.txt)
    list(APPEND SYSTEM_TEST_DIRS ${child})
  endif()
endforeach()

foreach(subdir ${SYSTEM_TEST_DIRS})
  add_subdirectory(${subdir})
endforeach()

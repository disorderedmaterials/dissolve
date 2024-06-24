# CMakeLists.txt for Dissolve Benchmarks

function(dissolve_add_benchmark)

  # Parse arguments
  set(options "")
  set(oneValueArgs SRC "")
  cmake_parse_arguments(DISSOLVE_BENCHMARK "${options}" "${oneValueArgs}" "" ${ARGN})

  # Check args
  if(NOT DEFINED DISSOLVE_BENCHMARK_SRC)
    message(FATAL_ERROR "Benchmark registered from directory '${CMAKE_CURRENT_LIST_DIR}' did not set a SRC file.")
  endif(NOT DEFINED DISSOLVE_BENCHMARK_SRC)
  # set(DISSOLVE_BENCHMARK_USE_TEST_DIRECTORY "${PROJECT_SOURCE_DIR}/benchmarks/data")

  # Strip path and extension from source file to get benchmark name
  get_filename_component(benchmark_file ${DISSOLVE_BENCHMARK_SRC} NAME_WE)
  get_filename_component(benchmark_file_dir "${CMAKE_CURRENT_LIST_DIR}" NAME)
  set(BENCHMARK_NAME "${benchmark_file_dir}_${benchmark_file}")
  message(STATUS "... Benchmark '${BENCHMARK_NAME}' from ${DISSOLVE_BENCHMARK_SRC} in directory '${CMAKE_CURRENT_LIST_DIR}'")
  message(STATUS "    ... working directory = ${DISSOLVE_BENCHMARK_USE_TEST_DIRECTORY}")

  # Register executable target
  add_executable(${BENCHMARK_NAME} ${DISSOLVE_BENCHMARK_SRC})
  target_compile_options(${BENCHMARK_NAME} PRIVATE -O3)
  target_include_directories(${BENCHMARK_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

  # Configure target
  target_include_directories(
    ${BENCHMARK_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/src ${PROJECT_BINARY_DIR}/src ${PROJECT_SOURCE_DIR} ${CONAN_INCLUDE_DIRS_GTEST}
                              ${CONAN_INCLUDE_DIRS_PUGIXML}
  )
  target_link_libraries(
    ${BENCHMARK_NAME}
    PUBLIC ${WHOLE_ARCHIVE_FLAG} ${BASIC_LINK_LIBS} ${MODULENOGUI_LINK_LIBS} ${NO_WHOLE_ARCHIVE_FLAG}
    PRIVATE ${CORE_LINK_LIBS} benchmark::benchmark
  )

endfunction()

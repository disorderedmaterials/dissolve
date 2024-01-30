macro(DISSOLVE_BENCHMARK NAME)
  set(benchmark_files ${benchmark_files} ${CMAKE_CURRENT_SOURCE_DIR}/${NAME}/${NAME}_benchmark.cpp)
endmacro(DISSOLVE_BENCHMARK)

dissolve_benchmark(calculate_gr)
dissolve_benchmark(box)
dissolve_benchmark(histogram)
dissolve_benchmark(array)
dissolve_benchmark(energy)
dissolve_benchmark(forces)
dissolve_benchmark(region)
dissolve_benchmark(math)

# create a single benchmark executable
add_executable(benchmarks benchmarks.cpp ${benchmark_files})

target_include_directories(benchmarks PRIVATE ${PROJECT_SOURCE_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

if(CONAN)
  target_link_libraries(
    benchmarks
    PRIVATE benchmark::benchmark
    PUBLIC ${WHOLE_ARCHIVE_FLAG} ${BASIC_LINK_LIBS} ${MODULENOGUI_LINK_LIBS} ${NO_WHOLE_ARCHIVE_FLAG}
    PRIVATE ${CORE_LINK_LIBS} ${THREADING_LINK_LIBS}
  )
else(CONAN)
  target_link_libraries(
    benchmarks
    PRIVATE benchmark::benchmark
    PUBLIC ${WHOLE_ARCHIVE_FLAG} ${BASIC_LINK_LIBS} ${MODULENOGUI_LINK_LIBS} ${NO_WHOLE_ARCHIVE_FLAG}
    PRIVATE antlr4-runtime ${EXTRA_LINK_LIBS} ${THREADING_LINK_LIBS}
  )
endif(CONAN)

target_compile_options(benchmarks PRIVATE -O3)

# Find all the input files
file(
  GLOB benchmarkFiles
  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
  "${CMAKE_CURRENT_SOURCE_DIR}/benchmark_files/*.txt"
)

# configure the input header to point to the directory containing the benchmark
set(BENCHMARK_PATH \"${CMAKE_CURRENT_BINARY_DIR}/\")
configure_file(benchmark_config.h.in benchmark_config.h @ONLY)

foreach(benchmarkFile ${benchmarkFiles})
  file(COPY ${benchmarkFile} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endforeach(benchmarkFile)

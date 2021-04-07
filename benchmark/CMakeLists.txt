macro(DISSOLVE_BENCHMARK NAME)
  set(benchmark_files ${benchmark_files} ${CMAKE_CURRENT_SOURCE_DIR}/${NAME}/${NAME}_benchmark.cpp)
endmacro(DISSOLVE_BENCHMARK)

dissolve_benchmark(calculate_gr)
dissolve_benchmark(box)

# create a single benchmark executable
add_executable(benchmarks benchmarks.cpp ${benchmark_files})

target_include_directories(benchmarks PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(
  benchmarks
  PRIVATE benchmark::benchmark
  PUBLIC ${WHOLE_ARCHIVE_FLAG} ${BASIC_LINK_LIBS}
         # Module 'nogui' libs
         ${MODULENOGUI_LINK_LIBS} ${NO_WHOLE_ARCHIVE_FLAG}
  PRIVATE # External libs
          antlr4-runtime ${EXTRA_LINK_LIBS} ${THREADING_LINK_LIBS}
)

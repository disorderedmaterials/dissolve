add_executable(benchmark_CalculateGR CalculateGR/benchmarks.cpp)
add_executable(benchmark_Box Box/benchmarks.cpp)

target_link_libraries(
  benchmark_CalculateGR
  PRIVATE benchmark::benchmark
  PUBLIC ${WHOLE_ARCHIVE_FLAG} ${BASIC_LINK_LIBS}
         # Module 'nogui' libs
         ${MODULENOGUI_LINK_LIBS} ${NO_WHOLE_ARCHIVE_FLAG}
  PRIVATE # External libs
          antlr4-runtime ${EXTRA_LINK_LIBS} ${THREADING_LINK_LIBS}
)
target_include_directories(benchmark_CalculateGR PRIVATE ${PROJECT_SOURCE_DIR}/src)

target_link_libraries(
  benchmark_Box
  PRIVATE benchmark::benchmark
  PUBLIC ${WHOLE_ARCHIVE_FLAG} ${BASIC_LINK_LIBS}
         # Module 'nogui' libs
         ${MODULENOGUI_LINK_LIBS} ${NO_WHOLE_ARCHIVE_FLAG}
  PRIVATE # External libs
          antlr4-runtime ${EXTRA_LINK_LIBS} ${THREADING_LINK_LIBS}
)
target_include_directories(benchmark_Box PRIVATE ${PROJECT_SOURCE_DIR}/src)

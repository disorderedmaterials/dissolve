add_executable("torch_torch" torch.cpp)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TORCH_CXX_FLAGS}")

target_include_directories(
  "torch_torch"
  PRIVATE ${PROJECT_SOURCE_DIR}/src ${PROJECT_BINARY_DIR}/src ${ANTLR4_INCLUDE_DIRS} ${ANTLR_OUTPUT_DIRS}
  ${CONAN_INCLUDE_DIRS_GTEST} ${CONAN_INCLUDE_DIRS_PUGIXML} ${Torch_INCLUDE_DIRS}
)


target_link_libraries(
  "torch_torch"
  PUBLIC ${WHOLE_ARCHIVE_FLAG} ${BASIC_LINK_LIBS} ${MODULENOGUI_LINK_LIBS} ${NO_WHOLE_ARCHIVE_FLAG}
  PRIVATE antlr4-runtime ${EXTRA_LINK_LIBS} ${TORCH_LIBRARIES} GTest::gtest_main
)

gtest_discover_tests("torch_torch" WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/tests")

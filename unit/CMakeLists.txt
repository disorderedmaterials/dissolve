# CMakeLists.txt for Unit Tests All tests must be single source file.

# Set policy for GoogleTest
cmake_policy(SET CMP0054 NEW)

find_package(GTest REQUIRED)

function(dissolve_unit_test src_file)
  # Strip path and extension from source file to get test name
  get_filename_component(NAME ${src_file} NAME_WE)
  set(filepath ${CMAKE_CURRENT_LIST_DIR})
  cmake_path(GET filepath FILENAME prefix)
  set(TEST_NAME "${prefix}_${NAME}")
  message(STATUS "... Unit test '${NAME}' ('${TEST_NAME}') in directory '${CMAKE_CURRENT_LIST_DIR}'")

  # Register executable target
  add_executable(${TEST_NAME} ${src_file})

  # Configure target
  target_include_directories(
    ${TEST_NAME}
    PRIVATE ${PROJECT_SOURCE_DIR}/src
            ${PROJECT_BINARY_DIR}/src
            ${ANTLR4_INCLUDE_DIRS}
            ${ANTLR_OUTPUT_DIRS}
            ${Qt6Widgets_INCLUDE_DIRS}
            ${CONAN_INCLUDE_DIRS_GTEST}
            ${CONAN_INCLUDE_DIRS_PUGIXML}
  )
  target_link_libraries(
    ${TEST_NAME}
    PUBLIC ${WHOLE_ARCHIVE_FLAG} ${BASIC_LINK_LIBS} models ${MODULENOGUI_LINK_LIBS} ${NO_WHOLE_ARCHIVE_FLAG}
    PRIVATE antlr4-runtime ${EXTRA_LINK_LIBS} GTest::gtest_main Qt6::Core Qt6::Widgets
    INTERFACE CONAN_PKG::fmt CONAN_PKG::CLI11
  )

  # Register the test
  gtest_discover_tests(${TEST_NAME} WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/tests")
endfunction()

# Add unit test subdirectories
add_subdirectory(algorithms)
add_subdirectory(classes)
if(GUI)
  add_subdirectory(gui)
endif(GUI)
add_subdirectory(io)
add_subdirectory(math)

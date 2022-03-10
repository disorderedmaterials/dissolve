# CMakeLists.txt for Unit Tests All tests must be single source file.

# Set policy for GoogleTest
cmake_policy(SET CMP0054 NEW)
find_package(GTest REQUIRED)

set(TEST_GUI FALSE)

function(dissolve_unit_test src_file)
  # Strip path and extension from source file to get test name
  get_filename_component(test_file ${src_file} NAME_WE)
  get_filename_component(test_file_dir "${CMAKE_CURRENT_LIST_DIR}" NAME)
  set(TEST_NAME "${test_file_dir}_${test_file}")
  message(STATUS "... Unit test '${TEST_NAME}' from ${src_file} in directory '${CMAKE_CURRENT_LIST_DIR}'")

  # Register executable target
  add_executable(${TEST_NAME} ${src_file})

  # Configure target
  target_include_directories(
    ${TEST_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/src ${PROJECT_BINARY_DIR}/src ${ANTLR4_INCLUDE_DIRS} ${ANTLR_OUTPUT_DIRS}
                         ${CONAN_INCLUDE_DIRS_GTEST} ${CONAN_INCLUDE_DIRS_PUGIXML}
  )
  if(CONAN)
    target_link_libraries(
      ${TEST_NAME}
      PUBLIC ${WHOLE_ARCHIVE_FLAG} ${BASIC_LINK_LIBS} ${MODULENOGUI_LINK_LIBS} ${NO_WHOLE_ARCHIVE_FLAG}
      PRIVATE antlr4-runtime ${EXTRA_LINK_LIBS} GTest::gtest_main
      INTERFACE CONAN_PKG::fmt CONAN_PKG::cli11
    )
  else(CONAN)
    target_link_libraries(
      ${TEST_NAME}
      PUBLIC ${WHOLE_ARCHIVE_FLAG} ${BASIC_LINK_LIBS} ${MODULENOGUI_LINK_LIBS} ${NO_WHOLE_ARCHIVE_FLAG}
      PRIVATE antlr4-runtime ${EXTRA_LINK_LIBS} GTest::gtest_main
    )
  endif(CONAN)

  if(TEST_GUI)
    target_include_directories(${TEST_NAME} PRIVATE ${Qt6Widgets_INCLUDE_DIRS})
    target_link_libraries(
      ${TEST_NAME}
      PUBLIC keywordwidgets models widgets
      PRIVATE Qt6::Core Qt6::Widgets
    )
  endif(TEST_GUI)

  # Register the test
  if(${ARGC} EQUAL "2")
    message(STATUS " test working dir = ${PROJECT_SOURCE_DIR}/tests/${ARGV1}")
    gtest_discover_tests(${TEST_NAME} WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/tests/${ARGV1}")
  else(${ARGC} EQUAL "2")
    message(STATUS " test working dir = ${PROJECT_SOURCE_DIR}/tests")
    gtest_discover_tests(${TEST_NAME} WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/tests")
  endif(${ARGC} EQUAL "2")
endfunction()

# Add unit test subdirectories
add_subdirectory(algorithms)
add_subdirectory(classes)
if(GUI)
  add_subdirectory(gui)
endif(GUI)
add_subdirectory(io)
add_subdirectory(math)
add_subdirectory(torch)

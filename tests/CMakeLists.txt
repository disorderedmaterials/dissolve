# CMakeLists.txt for Dissolve Tests

# Set policy for GoogleTest
cmake_policy(SET CMP0054 NEW)
include(FindGTest)
#find_package(GTest REQUIRED)

function(dissolve_add_test)

  # Parse arguments
  set(options GUI)
  set(oneValueArgs SRC USE_TEST_DIRECTORY)
  cmake_parse_arguments(DISSOLVE_UNIT_TEST "${options}" "${oneValueArgs}" "" ${ARGN})

  # Check args
  if(NOT DEFINED DISSOLVE_UNIT_TEST_SRC)
    message(FATAL_ERROR "Test registered from directory '${CMAKE_CURRENT_LIST_DIR}' did not set a SRC file.")
  endif(NOT DEFINED DISSOLVE_UNIT_TEST_SRC)
  if(NOT DEFINED DISSOLVE_UNIT_TEST_USE_TEST_DIRECTORY)
    set(DISSOLVE_UNIT_TEST_USE_TEST_DIRECTORY "${PROJECT_SOURCE_DIR}/tests/data")
  else(NOT DEFINED DISSOLVE_UNIT_TEST_USE_TEST_DIRECTORY)
    set(DISSOLVE_UNIT_TEST_USE_TEST_DIRECTORY "${PROJECT_SOURCE_DIR}/tests/${DISSOLVE_UNIT_TEST_USE_TEST_DIRECTORY}")
  endif(NOT DEFINED DISSOLVE_UNIT_TEST_USE_TEST_DIRECTORY)

  # Strip path and extension from source file to get test name
  get_filename_component(test_file ${DISSOLVE_UNIT_TEST_SRC} NAME_WE)
  get_filename_component(test_file_dir "${CMAKE_CURRENT_LIST_DIR}" NAME)
  set(TEST_NAME "${test_file_dir}_${test_file}")
  message(STATUS "... Unit test '${TEST_NAME}' from ${DISSOLVE_UNIT_TEST_SRC} in directory '${CMAKE_CURRENT_LIST_DIR}'")
  message(STATUS "    ... working directory = ${DISSOLVE_UNIT_TEST_USE_TEST_DIRECTORY}")

  # Register executable target
  add_executable(${TEST_NAME} ${DISSOLVE_UNIT_TEST_SRC})

  # Configure target
  target_include_directories(
    ${TEST_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/src ${PROJECT_BINARY_DIR}/src ${PROJECT_SOURCE_DIR} ${CONAN_INCLUDE_DIRS_GTEST}
                         ${CONAN_INCLUDE_DIRS_PUGIXML}
  )
  if(CONAN)
    target_link_libraries(
      ${TEST_NAME}
      PUBLIC ${WHOLE_ARCHIVE_FLAG} ${BASIC_LINK_LIBS} ${MODULENOGUI_LINK_LIBS} ${NO_WHOLE_ARCHIVE_FLAG}
      PRIVATE ${EXTRA_LINK_LIBS} GTest::gtest_main
      INTERFACE CONAN_PKG::fmt CONAN_PKG::cli11 CONAN_PKG::antlr4-cppruntime
    )
  else(CONAN)
    target_link_libraries(
      ${TEST_NAME}
      PUBLIC ${WHOLE_ARCHIVE_FLAG} ${BASIC_LINK_LIBS} ${MODULENOGUI_LINK_LIBS} ${NO_WHOLE_ARCHIVE_FLAG}
      PRIVATE antlr4-runtime ${EXTRA_LINK_LIBS} GTest::gtest_main
    )
  endif(CONAN)

  if(DISSOLVE_UNIT_TEST_GUI)
    target_include_directories(${TEST_NAME} PRIVATE ${Qt6Widgets_INCLUDE_DIRS})
    target_link_libraries(
      ${TEST_NAME}
      PUBLIC keywordWidgets models widgets
      PRIVATE Qt6::Core Qt6::Widgets
    )
  endif(DISSOLVE_UNIT_TEST_GUI)

  # Register the test
  gtest_discover_tests(${TEST_NAME} WORKING_DIRECTORY ${DISSOLVE_UNIT_TEST_USE_TEST_DIRECTORY})

endfunction()

# Add unit test subdirectories
add_subdirectory(algorithms)
add_subdirectory(classes)
add_subdirectory(ff)
if(GUI)
  add_subdirectory(gui)
endif(GUI)
add_subdirectory(io)
add_subdirectory(keywords)
add_subdirectory(math)
add_subdirectory(modules)
add_subdirectory(procedure)

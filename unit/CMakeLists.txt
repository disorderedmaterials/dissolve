# CMakeLists.txt for Unit Tests
# All tests must be single source file.

# Set policy for GoogleTest
CMAKE_POLICY(SET CMP0054 NEW)

# Get list of sources, from which we can construct the exe / test list.
file(GLOB_RECURSE tests_SRC "*.cpp")
find_package(GTest REQUIRED)

# Add executables and tests
foreach(SRCFILE "${tests_SRC}")
  # Strip path and extension from source file to get test name
  get_filename_component(NAME ${SRCFILE} NAME_WE)

  # Register executable target
  ADD_EXECUTABLE(${NAME} ${SRCFILE})

  # Configure target
  target_include_directories(${NAME} PRIVATE
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_BINARY_DIR}/src
    ${ANTLR4_INCLUDE_DIRS}
    ${ANTLR_OUTPUT_DIRS}
  )
  target_link_libraries(${NAME} PUBLIC
    ${CONAN_LIBS}

    ${WHOLE_ARCHIVE_FLAG}

    ${BASIC_LINK_LIBS}

    # Module 'nogui' libs
    ${MODULENOGUI_LINK_LIBS}

    ${NO_WHOLE_ARCHIVE_FLAG}

    PRIVATE

    antlr4-runtime

    ${EXTRA_LINK_LIBS}

    INTERFACE

    CONAN_PKG::fmt
    CONAN_PKG::CLI11
    CONAN_PKG::GTest
  )

  # Register the test
  GTEST_DISCOVER_TESTS(${NAME})
endforeach(SRCFILE)

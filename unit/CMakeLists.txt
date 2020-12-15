# CMakeLists.txt for Unit Tests
# All tests must be single source file.

# Get list of sources, from which we can construct the exe / test list.
file(GLOB_RECURSE tests_SRC "*.cpp")

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
    ${BASIC_LINK_LIBS}
    PRIVATE

    antlr4-runtime

    ${EXTRA_LINK_LIBS}

    INTERFACE

    CONAN_PKG::fmt
    CONAN_PKG::CLI11
  )

  # Register the test
  ADD_TEST(NAME ${NAME} COMMAND $<TARGET_FILE:${NAME}>)
endforeach(SRCFILE)

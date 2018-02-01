# Main Module Library
add_library(sanitycheckmodule
  core.cpp
  definition.cpp
  functions.cpp
  options.cpp
  process.cpp

  sanitycheck.h
)

include_directories(
../../
${CMAKE_SOURCE_DIR}
${CMAKE_BINARY_DIR}
${CMAKE_BINARY_DIR}/src
)

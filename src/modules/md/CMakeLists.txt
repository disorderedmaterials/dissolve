# Main Module Library
add_library(mdmodule
  core.cpp
  definition.cpp
  functions.cpp
  options.cpp
  process.cpp

  md.h
)

include_directories(
../../
${CMAKE_SOURCE_DIR}
${CMAKE_BINARY_DIR}
${CMAKE_BINARY_DIR}/src
)

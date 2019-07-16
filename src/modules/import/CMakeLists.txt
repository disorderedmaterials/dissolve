# Main Module Library
add_library(importmodule
  core.cpp
  definition.cpp
  options.cpp
  process.cpp

  import.h
)

include_directories(
../../
${CMAKE_SOURCE_DIR}
${CMAKE_BINARY_DIR}
${CMAKE_BINARY_DIR}/src
)

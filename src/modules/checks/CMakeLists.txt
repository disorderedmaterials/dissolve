# Main Module Library
add_library(checksmodule
  core.cpp
  data.cpp
  definition.cpp
  functions.cpp
  method.cpp
  options.cpp

  checks.h
)

include_directories(
../../
${CMAKE_SOURCE_DIR}
${CMAKE_BINARY_DIR}
${CMAKE_BINARY_DIR}/src
)

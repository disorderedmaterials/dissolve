# Main Module Library
add_library(molshakemodule
  core.cpp
  data.cpp
  definition.cpp
  functions.cpp
  method.cpp
  options.cpp

  molshake.h
)

include_directories(
../../
${CMAKE_SOURCE_DIR}
${CMAKE_BINARY_DIR}
${CMAKE_BINARY_DIR}/src
)

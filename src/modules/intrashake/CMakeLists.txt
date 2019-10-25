# Main Module Library
add_library(intrashakemodule
  core.cpp
  definition.cpp
  functions.cpp
  init.cpp
  process.cpp

  intrashake.h
)

include_directories(
../../
${CMAKE_SOURCE_DIR}
${CMAKE_BINARY_DIR}
${CMAKE_BINARY_DIR}/src
)

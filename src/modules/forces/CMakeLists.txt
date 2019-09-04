# Main Module Library
add_library(forcesmodule
  core.cpp
  definition.cpp
  functions.cpp
  init.cpp
  process.cpp

  forces.h
)

include_directories(
../../
${CMAKE_SOURCE_DIR}
${CMAKE_BINARY_DIR}
${CMAKE_BINARY_DIR}/src
)

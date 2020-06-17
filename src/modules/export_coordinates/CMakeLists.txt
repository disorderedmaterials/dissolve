# Main Module Library
add_library(exportcoordinatesmodule
  core.cpp
  definition.cpp
  init.cpp
  process.cpp

  exportcoords.h
)

include_directories(
../../
${CMAKE_SOURCE_DIR}
${CMAKE_BINARY_DIR}
${CMAKE_BINARY_DIR}/src
)

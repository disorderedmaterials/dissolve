# Main Module Library
add_library(importmodule
  core.cpp
  definition.cpp
  formats.cpp
  functions_coordinates.cpp
  functions_coordinates_dlpoly.cpp
  functions_coordinates_epsr.cpp
  functions_coordinates_xyz.cpp
  functions_data1d.cpp
  functions_forces.cpp
  options.cpp
  process.cpp

  formats.h
  import.h
)

include_directories(
../../
${CMAKE_SOURCE_DIR}
${CMAKE_BINARY_DIR}
${CMAKE_BINARY_DIR}/src
)

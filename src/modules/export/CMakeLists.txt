# Main Module Library
add_library(exportmodule
  core.cpp
  definition.cpp
  formats.cpp
  functions_coordinates.cpp
  functions_data2d.cpp
  options.cpp
  process.cpp

  export.h
  formats.h
)

include_directories(
../../
${CMAKE_SOURCE_DIR}
${CMAKE_BINARY_DIR}
${CMAKE_BINARY_DIR}/src
)

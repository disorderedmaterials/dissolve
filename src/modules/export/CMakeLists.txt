# Main Module Library
add_library(exportmodule
  core.cpp
  definition.cpp
  functions.cpp
  method.cpp
  options.cpp

  export.h
)

include_directories(
../../
${CMAKE_SOURCE_DIR}
${CMAKE_BINARY_DIR}
${CMAKE_BINARY_DIR}/src
)

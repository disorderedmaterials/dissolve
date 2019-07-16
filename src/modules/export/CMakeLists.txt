# Main Module Library
add_library(exportmodule
  core.cpp
  definition.cpp
  options.cpp
  process.cpp

  export.h
)

include_directories(
../../
${CMAKE_SOURCE_DIR}
${CMAKE_BINARY_DIR}
${CMAKE_BINARY_DIR}/src
)

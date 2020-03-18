# Main Module Library
add_library(exporttrajectorymodule
  core.cpp
  definition.cpp
  init.cpp
  process.cpp

  exporttraj.h
)

include_directories(
../../
${CMAKE_SOURCE_DIR}
${CMAKE_BINARY_DIR}
${CMAKE_BINARY_DIR}/src
)

# Main Module Library
add_library(exportpairpotentialsmodule
  core.cpp
  definition.cpp
  init.cpp
  process.cpp

  exportpp.h
)

include_directories(
../../
${CMAKE_SOURCE_DIR}
${CMAKE_BINARY_DIR}
${CMAKE_BINARY_DIR}/src
)

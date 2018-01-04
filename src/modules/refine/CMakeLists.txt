add_library(testmodule
  core.cpp
  data.cpp
  definition.cpp
  functions.cpp
  method.cpp
  options.cpp

  test.h
)

include_directories(
../../
${CMAKE_SOURCE_DIR}
${CMAKE_BINARY_DIR}
${CMAKE_BINARY_DIR}/src
)

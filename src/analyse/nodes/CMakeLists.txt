add_library(analysisnodes
  node.cpp
  select.cpp
  sequence.cpp

  node.h
  nodes.h
  select.h
  sequence.h
)

include_directories(
../
../../
${CMAKE_SOURCE_DIR}
${CMAKE_BINARY_DIR}
${CMAKE_BINARY_DIR}/src
)


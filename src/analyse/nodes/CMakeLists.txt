add_library(analysisnodes
  calculate.cpp
  collect1d.cpp
  exclude.cpp
  node.cpp
  normalise1d.cpp
  select.cpp
  sequence.cpp

  calculate.h
  collect1d.h
  exclude.h
  node.h
  nodes.h
  normalise1d.h
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


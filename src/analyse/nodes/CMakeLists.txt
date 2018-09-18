add_library(analysisnodes
  calculate.cpp
  collect1d.cpp
  collect2d.cpp
  collect3d.cpp
  exclude.cpp
  node.cpp
  normalise1d.cpp
  normalise2d.cpp
  normalise3d.cpp
  select.cpp
  selectbase.cpp
  sequence.cpp

  calculate.h
  collect1d.h
  collect2d.h
  collect3d.h
  exclude.h
  node.h
  nodes.h
  normalise1d.h
  normalise2d.h
  normalise3d.h
  select.h
  selectbase.h
  sequence.h
)

include_directories(
../
../../
${CMAKE_SOURCE_DIR}
${CMAKE_BINARY_DIR}
${CMAKE_BINARY_DIR}/src
)


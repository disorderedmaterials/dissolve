add_library(analysisnodes
  calculate.cpp
  collect1d.cpp
  collect2d.cpp
  collect3d.cpp
  exclude.cpp
  node.cpp
  process1d.cpp
  process2d.cpp
  process3d.cpp
  select.cpp
  sequence.cpp

  calculate.h
  collect1d.h
  collect2d.h
  collect3d.h
  exclude.h
  node.h
  nodes.h
  process1d.h
  process2d.h
  process3d.h
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


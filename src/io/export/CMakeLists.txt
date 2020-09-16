add_library(export
  coordinates.cpp
  data1d.cpp
  data2d.cpp
  data3d.cpp
  forces.cpp
  pairpotential.cpp
  trajectory.cpp

  coordinates.h
  data1d.h
  data2d.h
  data3d.h
  forces.h
  pairpotential.h
  trajectory.h
  )

include_directories(
  ${PROJECT_SOURCE_DIR}/src
  ${PROJECT_BINARY_DIR}/src
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_BINARY_DIR}
  ${CMAKE_BINARY_DIR}/src
)

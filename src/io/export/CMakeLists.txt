add_library(export
  coordinates.cpp
  data1d.cpp
  data2d.cpp
  pairpotential.cpp
  trajectory.cpp

  coordinates.h
  data1d.h
  data2d.h
  pairpotential.h
  trajectory.h
  )

include_directories(
../
${CMAKE_SOURCE_DIR}
${CMAKE_BINARY_DIR}
${CMAKE_BINARY_DIR}/src
)

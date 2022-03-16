add_library(
  export
  coordinates.cpp
  data1d.cpp
  data2d.cpp
  data3d.cpp
  forces.cpp
  pairpotential.cpp
  trajectory.cpp
  values.cpp
  coordinates.h
  data1d.h
  data2d.h
  data3d.h
  forces.h
  pairpotential.h
  trajectory.h
  values.h
)

target_include_directories(export PRIVATE ${PROJECT_SOURCE_DIR}/src ${PROJECT_BINARY_DIR}/src)
target_link_libraries(export PRIVATE base)

add_library(
  export
  coordinates.cpp
  data1D.cpp
  data2D.cpp
  data3D.cpp
  forces.cpp
  pairPotential.cpp
  species.cpp
  trajectory.cpp
  values.cpp
  dlPolyControl.cpp
  dlPolyField.cpp
  coordinates.h
  data1D.h
  data2D.h
  data3D.h
  forces.h
  pairPotential.h
  species.h
  trajectory.h
  values.h
  dlPolyControl.h
  dlPolyField.h
)

target_include_directories(export PRIVATE ${PROJECT_SOURCE_DIR}/src ${PROJECT_BINARY_DIR}/src)
target_link_libraries(export PRIVATE base)

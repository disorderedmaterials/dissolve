add_library(
  export
  data1D.cpp
  data2D.cpp
  data3D.cpp
  forces.cpp
  pairPotential.cpp
  species.cpp
  values.cpp
  data1D.h
  data2D.h
  data3D.h
  forces.h
  pairPotential.h
  species.h
  values.h
)

target_include_directories(export PRIVATE ${PROJECT_SOURCE_DIR}/src ${PROJECT_BINARY_DIR}/src)
target_link_libraries(export PRIVATE base)

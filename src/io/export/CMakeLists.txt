add_library(
  export
  data1D.cpp
  data2D.cpp
  data3D.cpp
  pairPotential.cpp
  species.cpp
  data1D.h
  data2D.h
  data3D.h
  pairPotential.h
  species.h
)

target_include_directories(export PRIVATE ${PROJECT_SOURCE_DIR}/src ${PROJECT_BINARY_DIR}/src)
target_link_libraries(export PRIVATE base)

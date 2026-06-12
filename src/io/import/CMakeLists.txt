add_library(
  import
  coordinates.cpp
  coordinates_dlpoly.cpp
  coordinates_epsr.cpp
  coordinates_moscito.cpp
  coordinates_xyz.cpp
  data1D.cpp
  data1D_histo.cpp
  data1D_mint.cpp
  data1D_xy.cpp
  data2D.cpp
  data2D_cartesian.cpp
  data3D.cpp
  data3D_pdens.cpp
  forces.cpp
  forces_dlpoly.cpp
  forces_moscito.cpp
  forces_simple.cpp
  trajectory.cpp
  trajectory_dlpoly.cpp
  coordinates.h
  data1D.h
  data2D.h
  data3D.h
  forces.h
  trajectory.h
)

target_include_directories(import PRIVATE ${PROJECT_SOURCE_DIR}/src ${PROJECT_BINARY_DIR}/src)

target_link_libraries(import base nodes)

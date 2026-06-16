add_library(
  import
  data2D.cpp
  data2D_cartesian.cpp
  data3D.cpp
  data3D_pdens.cpp
  data2D.h
  data3D.h
)

target_include_directories(import PRIVATE ${PROJECT_SOURCE_DIR}/src ${PROJECT_BINARY_DIR}/src)

target_link_libraries(import base nodes)

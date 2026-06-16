add_library(
  import
  data2D.cpp
  data2D_cartesian.cpp
  data2D.h
  data2D.h
)

target_include_directories(import PRIVATE ${PROJECT_SOURCE_DIR}/src ${PROJECT_BINARY_DIR}/src)

target_link_libraries(import base nodes)

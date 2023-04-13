add_library(
  kernels
  base.cpp
  energy.cpp
  force.cpp
  geometry.cpp
  base.h
  energy.h
  force.h
  geometry.h
)

target_include_directories(kernels PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(kernels PUBLIC base ${THREADING_LINK_LIBS})

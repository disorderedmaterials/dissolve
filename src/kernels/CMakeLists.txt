add_library(
  kernels
  base.cpp
  energy.cpp
  force.cpp
  geometry.cpp
  species.cpp
  base.h
  common.h
  energy.h
  force.h
  geometry.h
  species.h
)

target_include_directories(kernels PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(kernels PUBLIC base ${THREADING_LINK_LIBS})

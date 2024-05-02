add_library(
  kernels
  base.cpp
  energy.cpp
  externalPotentials.cpp
  force.cpp
  geometry.cpp
  producer.cpp
  base.h
  energy.h
  externalPotentials.h
  force.h
  geometry.h
  producer.h
)

target_include_directories(kernels PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(kernels PUBLIC base ${THREADING_LINK_LIBS})

add_subdirectory(potentials)

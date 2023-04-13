add_library(
  kernels
  energy.cpp
  force.cpp
  energy.h
  force.h
)

target_include_directories(kernels PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(kernels PUBLIC base ${THREADING_LINK_LIBS})

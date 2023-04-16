add_library(
  potentials
  base.cpp
  base.h
)

target_include_directories(potentials PRIVATE ${PROJECT_SOURCE_DIR}/src)


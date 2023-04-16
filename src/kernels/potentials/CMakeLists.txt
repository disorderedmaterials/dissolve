add_library(
  potentials
  base.cpp
  spherical.cpp
  base.h
  spherical.h
)

target_include_directories(potentials PRIVATE ${PROJECT_SOURCE_DIR}/src ${CONAN_INCLUDE_DIRS})


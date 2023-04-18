add_library(
  potentials
  base.cpp
  producer.cpp
  spherical.cpp
  types.cpp
  base.h
  producer.h
  spherical.h
  types.h
)

target_include_directories(potentials PRIVATE ${PROJECT_SOURCE_DIR}/src ${CONAN_INCLUDE_DIRS})

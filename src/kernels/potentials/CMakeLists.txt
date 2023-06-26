add_library(
  potentials
  base.cpp
  directional.cpp
  producer.cpp
  simple.cpp
  types.cpp
  base.h
  directional.h
  producer.h
  simple.h
  types.h
)

target_include_directories(potentials PRIVATE ${PROJECT_SOURCE_DIR}/src ${CONAN_INCLUDE_DIRS})

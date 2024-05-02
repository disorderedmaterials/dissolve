add_library(
  potentials
  base.cpp
  directional.cpp
  producer.cpp
  regional.cpp
  restraint.cpp
  simple.cpp
  types.cpp
  base.h
  directional.h
  producer.h
  regional.h
  restraint.h
  simple.h
  types.h
)

target_include_directories(potentials PRIVATE ${PROJECT_SOURCE_DIR}/src ${CONAN_INCLUDE_DIRS})

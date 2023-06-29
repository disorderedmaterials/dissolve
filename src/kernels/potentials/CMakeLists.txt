add_library(
  potentials
  base.cpp
  producer.cpp
  restraint.cpp
  simple.cpp
  types.cpp
  base.h
  producer.h
  restraint.h
  simple.h
  types.h
)

target_include_directories(potentials PRIVATE ${PROJECT_SOURCE_DIR}/src ${CONAN_INCLUDE_DIRS})

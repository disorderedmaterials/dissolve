add_library(
  potentials
  base.cpp
  producer.cpp
  simple.cpp
  simplerestraint.cpp
  types.cpp
  base.h
  producer.h
  simple.h
  simplerestraint.h
  types.h
)

target_include_directories(potentials PRIVATE ${PROJECT_SOURCE_DIR}/src ${CONAN_INCLUDE_DIRS})

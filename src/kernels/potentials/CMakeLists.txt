add_library(
  potentials
  base.cpp
  cylindrical.cpp
  producer.cpp
  regional.cpp
  restraint.cpp
  spherical.cpp
  types.cpp
  base.h
  cylindrical.h
  producer.h
  regional.h
  restraint.h
  spherical.h
  types.h
)

target_include_directories(potentials PRIVATE ${PROJECT_SOURCE_DIR}/src ${CONAN_INCLUDE_DIRS})

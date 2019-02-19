add_library(data
  atomicmass.cpp
  elements.cpp
  ff.cpp
  isotopes.cpp

  atomicmass.h
  elements.h
  ff.h
  isotopes.h
)

include_directories(
../
${CMAKE_SOURCE_DIR}
${CMAKE_BINARY_DIR}
${CMAKE_BINARY_DIR}/src
)

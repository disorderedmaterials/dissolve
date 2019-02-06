add_library(data
  atomicmass.cpp
  elements.cpp
  isotopes.cpp
  uff.cpp

  atomicmass.h
  elements.h
  ff.h
  isotopes.h
  uff.h
)

include_directories(
../
${CMAKE_SOURCE_DIR}
${CMAKE_BINARY_DIR}
${CMAKE_BINARY_DIR}/src
)

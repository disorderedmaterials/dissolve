add_library(data
  atomicmass.cpp
  elementcolours.cpp
  elements.cpp
  isotopes.cpp
  
  atomicmass.h
  elementcolours.h
  elements.h
  isotopes.h
)

include_directories(
../
${CMAKE_SOURCE_DIR}
${CMAKE_BINARY_DIR}
${CMAKE_BINARY_DIR}/src
)

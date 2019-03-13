add_library(data
  atomicmass.cpp
  elementcolours.cpp
  elements.cpp
  ff.cpp
  ffatomtype.cpp
  fflibrary.cpp
  isotopes.cpp

  atomicmass.h
  elementcolours.h
  elements.h
  ff.h
  ffatomtype.h
  fflibrary.h
  isotopes.h
)

include_directories(
../
${CMAKE_SOURCE_DIR}
${CMAKE_BINARY_DIR}
${CMAKE_BINARY_DIR}/src
)

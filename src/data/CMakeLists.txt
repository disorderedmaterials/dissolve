add_library(data
  atomicmass.cpp
  elementcolours.cpp
  elements.cpp
  ff.cpp
  ffangleterm.cpp
  ffatomtype.cpp
  ffbondterm.cpp
  fflibrary.cpp
  fftorsionterm.cpp
  isotopes.cpp

  atomicmass.h
  elementcolours.h
  elements.h
  ff.h
  ffangleterm.h
  ffatomtype.h
  ffbondterm.h
  fflibrary.h
  fftorsionterm.h
  isotopes.h
)

include_directories(
../
${CMAKE_SOURCE_DIR}
${CMAKE_BINARY_DIR}
${CMAKE_BINARY_DIR}/src
)

add_subdirectory(ff)

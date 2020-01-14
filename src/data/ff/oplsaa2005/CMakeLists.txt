add_library(ff_oplsaa2005
  # Core
  atomtypes.cpp
  base.cpp

  atomtypes.h
  base.h

  # Aromatics
  aromatics.cpp
  aromatics.h

  # Noble Gases
  noblegases.cpp
  noblegases.h
)

include_directories(
../
${CMAKE_SOURCE_DIR}
${CMAKE_BINARY_DIR}
${CMAKE_BINARY_DIR}/src
)

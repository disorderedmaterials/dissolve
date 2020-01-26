add_library(ff_oplsaa2005
  # Core
  atomtypes.cpp
  intramolecular.cpp
  base.cpp

  atomtypes.h
  intramolecular.h
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

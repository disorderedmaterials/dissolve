add_library(ff_oplsaa2005
  # Core
  atomtypes.cpp
  intramolecular.cpp
  base.cpp

  base.h

  # Alcohols
  alcohols.cpp
  alcohols.h

  # Alkanes
  alkanes.cpp
  alkanes.h

  # Alkenes
  alkenes.cpp
  alkenes.h

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

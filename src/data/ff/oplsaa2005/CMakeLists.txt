add_library(ff_oplsaa2005
  atomtypes.cpp
  noblegases.cpp

  atomtypes.h
  noblegases.h
)

include_directories(
../
${CMAKE_SOURCE_DIR}
${CMAKE_BINARY_DIR}
${CMAKE_BINARY_DIR}/src
)

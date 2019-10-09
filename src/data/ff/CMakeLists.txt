add_library(ff
  # OPLS-AA (2005)
  oplsaa2005_atomtypes.cpp
  oplsaa2005_noblegases.cpp
  oplsaa2005_atomtypes.h
  oplsaa2005_noblegases.h

  # SPC/Fw
  spcfw.cpp
  spcfw.h

  # UFF
  uff.cpp
  uffatomtype.cpp
  uff.h
  uffatomtype.h
)

include_directories(
../
${CMAKE_SOURCE_DIR}
${CMAKE_BINARY_DIR}
${CMAKE_BINARY_DIR}/src
)

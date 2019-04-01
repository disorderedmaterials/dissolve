add_library(ff
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

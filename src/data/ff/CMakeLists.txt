add_library(ff
  # Kulmala 2010
  kulmala2010.cpp
  kulmala2010.h

  # SPC/Fw
  spcfw.cpp
  spcfw.h

  # UFF
  uff.cpp
  uffatomtype.cpp
  uff.h
  uffatomtype.h
)

add_subdirectory(ludwig)
add_subdirectory(oplsaa2005)

include_directories(
../
${CMAKE_SOURCE_DIR}
${CMAKE_BINARY_DIR}
${CMAKE_BINARY_DIR}/src
)

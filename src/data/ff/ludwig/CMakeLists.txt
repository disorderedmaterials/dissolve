add_library(ff_ludwig
  ntf2.cpp
  py4oh.cpp
  py5.cpp

  ntf2.h
  py4oh.h
  py5.h
)

include_directories(
../
${CMAKE_SOURCE_DIR}
${CMAKE_BINARY_DIR}
${CMAKE_BINARY_DIR}/src
)

add_library(ff
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

add_library(ff_ludwig
  ntf2_ludwig.cpp
  py4oh_ludwig.cpp
  py5_ludwig.cpp

  ntf2_ludwig.h
  py4oh_ludwig.h
  py5_ludwig.h
)

include_directories(
../
${CMAKE_SOURCE_DIR}
${CMAKE_BINARY_DIR}
${CMAKE_BINARY_DIR}/src
)

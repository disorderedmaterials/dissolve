add_subdirectory(nodes)

add_library(procedure
  procedure.cpp
  nodevalue.cpp
  
  procedure.h
  nodevalue.h
)

include_directories(
../
${CMAKE_SOURCE_DIR}
${CMAKE_BINARY_DIR}
${CMAKE_BINARY_DIR}/src
)

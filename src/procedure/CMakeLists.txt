add_subdirectory(nodes)

add_library(procedure
  procedure.cpp
  nodescopestack.cpp
  
  procedure.h
  nodescopestack.h
)

include_directories(
../
${CMAKE_SOURCE_DIR}
${CMAKE_BINARY_DIR}
${CMAKE_BINARY_DIR}/src
)

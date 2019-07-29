add_subdirectory(nodes)

add_library(procedure
  procedure.cpp
  nodevalue.cpp
  nodescope.cpp
  nodescopestack.cpp
  
  procedure.h
  nodevalue.h
  nodescope.h
  nodescopestack.h
)

include_directories(
../
${CMAKE_SOURCE_DIR}
${CMAKE_BINARY_DIR}
${CMAKE_BINARY_DIR}/src
)

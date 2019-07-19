add_subdirectory(nodes)

add_library(procedure
  procedure.cpp
  nodescope.cpp
  nodescopestack.cpp
  
  procedure.h
  nodescope.h
  nodescopestack.h
)

include_directories(
../
${CMAKE_SOURCE_DIR}
${CMAKE_BINARY_DIR}
${CMAKE_BINARY_DIR}/src
)

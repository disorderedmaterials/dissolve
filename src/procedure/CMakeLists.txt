add_subdirectory(nodes)

add_library(procedure
  procedurer.cpp
  nodecontextstack.cpp
  
  procedurer.h
  nodecontextstack.h
)

include_directories(
../
${CMAKE_SOURCE_DIR}
${CMAKE_BINARY_DIR}
${CMAKE_BINARY_DIR}/src
)

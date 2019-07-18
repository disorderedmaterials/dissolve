add_subdirectory(nodes)

add_library(analyse
  analyser.cpp
  nodecontextstack.cpp
  
  analyser.h
  nodecontextstack.h
)

include_directories(
../
${CMAKE_SOURCE_DIR}
${CMAKE_BINARY_DIR}
${CMAKE_BINARY_DIR}/src
)

add_subdirectory(nodes)

add_library(analyse
  analyser.cpp
  sitecontext.cpp
  sitecontextstack.cpp
  sitereference.cpp
  
  analyser.h
  sitecontext.h
  sitecontextstack.h
  sitereference.h
)

include_directories(
../
${CMAKE_SOURCE_DIR}
${CMAKE_BINARY_DIR}
${CMAKE_BINARY_DIR}/src
)

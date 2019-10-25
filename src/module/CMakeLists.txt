add_library(module
  group.cpp
  groups.cpp
  layer.cpp
  list.cpp
  module.cpp

  group.h
  groups.h
  layer.h
  list.h
  module.h
)

include_directories(
../
${CMAKE_SOURCE_DIR}
${CMAKE_BINARY_DIR}
${CMAKE_BINARY_DIR}/src
)

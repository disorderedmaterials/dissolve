add_library(genericitems
  base.cpp
  item.cpp
  list.cpp
  
  base.h
  item.h
  items.h
  list.h
  listhelper.h
)

include_directories(
../
${CMAKE_SOURCE_DIR}
${CMAKE_BINARY_DIR}
${CMAKE_BINARY_DIR}/src
)

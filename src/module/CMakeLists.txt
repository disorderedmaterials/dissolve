add_library(module
  group.cpp
  groups.cpp
  keywordbase.cpp
  keywordgroup.cpp
  keywordlist.cpp
  layer.cpp
  list.cpp
  module.cpp

  group.h
  groups.h
  keywordbase.h
  keyworddata.h
  keywordgroup.h
  keywordlist.h
  keywordtypes.h
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

# Subdirectories
add_subdirectory(keywordtypes)

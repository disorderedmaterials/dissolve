add_library(module
  module.cpp
  keywordbase.cpp
  keywordlist.cpp
  list.cpp
  registry.cpp

  module.h
  list.h
  registry.h
  keywordbase.h
  keyworddata.h
  keywordlist.h
  list.h
  registry.h
  keywordtypes.h
)

include_directories(
../
${CMAKE_SOURCE_DIR}
${CMAKE_BINARY_DIR}
${CMAKE_BINARY_DIR}/src
)

# Subdirectories
add_subdirectory(keywordtypes)

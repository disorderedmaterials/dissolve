add_library(module
  module.cpp
  group.cpp
  groups.cpp
  keywordbase.cpp
  keywordlist.cpp
  list.cpp
  reference.cpp

  module.h
  group.h
  groups.h
  list.h
  keywordbase.h
  keyworddata.h
  keywordlist.h
  list.h
  reference.h
  registrar.h
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

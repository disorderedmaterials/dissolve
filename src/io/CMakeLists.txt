add_library(io
  fileandformat.cpp

  fileandformat.h
)

include_directories(
  ../
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_BINARY_DIR}
  ${CMAKE_BINARY_DIR}/src
)

# Subdirectories
add_subdirectory(export)
add_subdirectory(import)

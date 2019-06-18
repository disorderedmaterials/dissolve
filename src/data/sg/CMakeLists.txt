add_library(sg
  generator.h
  sginfo.h

  generator.cpp 
  sgclib.c
  sgfind.c
  sghkl.c
  sgio.c
  sgsi.c
)

include_directories(
  ../../
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_BINARY_DIR}
  ${CMAKE_BINARY_DIR}/src
)


add_library(
  sg
  generator.h
  sginfo.h
  generator.cpp
  sgclib.c
  sgfind.c
  sghkl.c
  sgio.c
  sgsi.c
)

include_directories(sg PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(sg PRIVATE base)

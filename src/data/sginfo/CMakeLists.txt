add_library(
  sginfo
  generator.h
  sginfo.h
  generator.cpp
  sgclib.c
  sgfind.c
  sghkl.c
  sgio.c
  sgsi.c
)

include_directories(sginfo PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(sginfo PRIVATE base)

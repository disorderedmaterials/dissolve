add_library(sginfo sginfo.h sgclib.c sgfind.c sghkl.c sgio.c sgsi.c)

target_include_directories(sginfo PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(sginfo PRIVATE base)

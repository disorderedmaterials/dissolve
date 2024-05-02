add_subdirectory(nodes)

add_library(procedure procedure.cpp nodeValue.cpp procedure.h nodeValue.h)

target_include_directories(procedure PRIVATE ${PROJECT_SOURCE_DIR}/src)

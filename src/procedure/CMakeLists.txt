add_subdirectory(nodes)

add_library(procedure procedure.cpp nodevalue.cpp procedure.h nodevalue.h)

include_directories(procedure PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(procedure PRIVATE CONAN_PKG::fmt)

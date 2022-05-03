add_subdirectory(nodes)

add_library(procedure procedure.cpp nodevalue.cpp procedure.h nodevalue.h)

target_include_directories(procedure PRIVATE ${PROJECT_SOURCE_DIR}/src)
if(CONAN)
  target_link_libraries(procedure PRIVATE CONAN_PKG::fmt)
else(CONAN)
  target_link_libraries(procedure PRIVATE fmt::fmt)
endif(CONAN)

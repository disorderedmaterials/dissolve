add_subdirectory(nodes)

add_library(procedure procedure.cpp nodeValue.cpp procedure.h nodeValue.h)

target_include_directories(procedure PRIVATE ${PROJECT_SOURCE_DIR}/src)
if(CONAN)
  target_link_libraries(procedure PRIVATE ${THREADING_LINK_LIBS})
else(CONAN)
  target_link_libraries(procedure PRIVATE fmt::fmt ${THREADING_LINK_LIBS})
endif(CONAN)

add_library(unittestdata INTERFACE species.h)

target_include_directories(classes PRIVATE ${PROJECT_SOURCE_DIR}/src)
if(CONAN)
  target_link_libraries(classes PUBLIC base CONAN_PKG::toml11 ${THREADING_LINK_LIBS})
else(CONAN)
  target_link_libraries(classes PUBLIC base toml11::toml11 ${THREADING_LINK_LIBS})
endif(CONAN)

add_library(task task.cpp context.cpp task.h context.h)

target_include_directories(task PRIVATE ${PROJECT_SOURCE_DIR}/src)
if(CONAN)
  target_link_libraries(task PRIVATE CONAN_PKG::fmt ${THREADING_LINK_LIBS})
else(CONAN)
  target_link_libraries(task PRIVATE fmt::fmt ${THREADING_LINK_LIBS})
endif(CONAN)

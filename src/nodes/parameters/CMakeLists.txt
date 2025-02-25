add_library(
  parameters
  parameter.h
)

target_include_directories(parameters PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(parameters PUBLIC base ${THREADING_LINK_LIBS})

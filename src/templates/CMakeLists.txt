add_library(
  templates
  vector3.cpp
  vector3.h
)

target_include_directories(templates PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(templates PRIVATE base)

add_library(
  module
  context.cpp
  group.cpp
  groups.cpp
  layer.cpp
  module.cpp
  context.h
  group.h
  groups.h
  layer.h
  module.h
  types.h
)

target_include_directories(module PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(module PRIVATE base)

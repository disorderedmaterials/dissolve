add_library(
  module
  group.cpp
  groups.cpp
  layer.cpp
  module.cpp
  group.h
  groups.h
  layer.h
  module.h
)

include_directories(module PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(module PRIVATE base)

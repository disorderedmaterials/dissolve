add_library(
  items
  deserialisers.cpp
  legacy.cpp
  list.cpp
  producers.cpp
  searchers.cpp
  serialisers.cpp
  deserialisers.h
  legacy.h
  list.h
  producers.h
  serialisers.h
)

target_include_directories(items PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(items PRIVATE base)

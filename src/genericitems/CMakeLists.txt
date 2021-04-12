add_library(
  genericitems
  deserialisers.cpp
  list.cpp
  producers.cpp
  searchers.cpp
  serialisers.cpp
  deserialisers.h
  list.h
  producers.h
  serialisers.h
)

include_directories(genericitems PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(genericitems PRIVATE base)

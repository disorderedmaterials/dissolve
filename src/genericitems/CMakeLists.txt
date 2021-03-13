add_library(
  genericitems
  broadcasters.cpp
  deserialisers.cpp
  list.cpp
  producers.cpp
  serialisers.cpp
  deserialisers.h
  list.h
  producers.h
  serialisers.h
)

include_directories(genericitems PRIVATE ${PROJECT_SOURCE_DIR}/src)

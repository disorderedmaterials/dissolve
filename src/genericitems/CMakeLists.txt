add_library(
  genericitems
  base.cpp
  broadcasters.cpp
  deserialisers.cpp
  item.cpp
  list.cpp
  producers.cpp
  serialisers.cpp
  base.h
  deserialisers.h
  item.h
  items.h
  list.h
  producers.h
  serialisers.h
)

include_directories(genericitems PRIVATE ${PROJECT_SOURCE_DIR}/src)

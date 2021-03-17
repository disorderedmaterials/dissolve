add_library(
  genericitems
  base.cpp
  item.cpp
  list.cpp
  base.h
  item.h
  items.h
  list.h
)

include_directories(genericitems PRIVATE ${PROJECT_SOURCE_DIR}/src)

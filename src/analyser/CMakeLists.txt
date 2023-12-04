add_library(
  analyser
  siteFilter.cpp
  siteFilter.h
  siteSelector.cpp
  siteSelector.h
)

target_include_directories(
  analyser PRIVATE ${PROJECT_SOURCE_DIR}/src ${PROJECT_BINARY_DIR}/src
)

target_link_libraries(analyser base)

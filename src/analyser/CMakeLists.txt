add_library(
  analyser
  dataExporter.h
  dataNormaliser1D.cpp
  dataNormaliser1D.h
  dataNormaliser2D.cpp
  dataNormaliser2D.h
  dataNormaliser3D.cpp
  dataNormaliser3D.h
  dataNormaliserBase.h
  siteFilter.cpp
  siteFilter.h
  siteSelector.cpp
  siteSelector.h
  typeDefs.h
)

target_include_directories(analyser PRIVATE ${PROJECT_SOURCE_DIR}/src ${PROJECT_BINARY_DIR}/src)

target_link_libraries(analyser base)

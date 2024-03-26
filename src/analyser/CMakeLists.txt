add_library(
  analyser
  dataExporter.h
  dataOperator1D.cpp
  dataOperator1D.h
  dataOperator2D.cpp
  dataOperator2D.h
  dataOperator3D.cpp
  dataOperator3D.h
  dataOperatorBase.h
  siteFilter.cpp
  siteFilter.h
  siteSelector.cpp
  siteSelector.h
  typeDefs.h
)

target_include_directories(analyser PRIVATE ${PROJECT_SOURCE_DIR}/src ${PROJECT_BINARY_DIR}/src)

target_link_libraries(analyser base)

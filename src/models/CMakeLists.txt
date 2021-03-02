add_library(
  models
  xmlAngleModel.cpp
  xmlAtomModel.cpp
  xmlBondModel.cpp
  xmlAngleModel.h
  xmlAtomModel.h
  xmlBondModel.h
)

target_include_directories(
  models PRIVATE ${PROJECT_SOURCE_DIR}/src ${PROJECT_BINARY_DIR}/src ${Qt5Widgets_INCLUDE_DIRS} ${FREETYPE_INCLUDE_DIRS}
)
# target_link_libraries(models keywordwidgets charts delegates widgets components)

include_directories(models PRIVATE ${PROJECT_SOURCE_DIR}/src)

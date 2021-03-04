set(
  models_SRCS
  xmlAngleModel.cpp
  xmlAtomModel.cpp
  xmlBondModel.cpp
  xmlTorsionModel.cpp
)

qt_wrap_cpp(models_SRCS
  xmlAngleModel.h
  xmlAtomModel.h
  xmlBondModel.h
  xmlTorsionModel.h
  )

add_library(models ${models_SRCS})

include_directories(models PRIVATE ${PROJECT_SOURCE_DIR}/src)

target_include_directories(
  models PRIVATE ${PROJECT_SOURCE_DIR}/src ${PROJECT_BINARY_DIR}/src ${Qt5Widgets_INCLUDE_DIRS} ${FREETYPE_INCLUDE_DIRS}
)
# target_link_libraries(models keywordwidgets charts delegates widgets components)

include_directories(models PRIVATE ${PROJECT_SOURCE_DIR}/src)

set(models_MOC_HDRS
    dataManagerModel.h
    xmlAngleModel.h
    xmlAtomModel.h
    xmlBondModel.h
    xmlImproperModel.h
    xmlTreeModel.h
    xmlTorsionModel.h
)

qt5_wrap_cpp(models_MOC_SRCS ${models_MOC_HDRS})

set(models_SRCS
    dataManagerModel.cpp
    xmlAngleModel.cpp
    xmlAtomModel.cpp
    xmlBondModel.cpp
    xmlImproperModel.cpp
    xmlTreeModel.cpp
    xmlTorsionModel.cpp
)

qt_wrap_cpp(
  models_SRCS
  ${models_MOC_HDRS}
)

add_library(models ${models_SRCS} ${models_MOC_SRCS})

include_directories(models PRIVATE ${PROJECT_SOURCE_DIR}/src)

target_include_directories(
  models PRIVATE ${PROJECT_SOURCE_DIR}/src ${PROJECT_BINARY_DIR}/src ${Qt5Widgets_INCLUDE_DIRS} ${FREETYPE_INCLUDE_DIRS}
)
target_link_libraries(models keywordwidgets charts delegates widgets components gui)

include_directories(models PRIVATE ${PROJECT_SOURCE_DIR}/src)

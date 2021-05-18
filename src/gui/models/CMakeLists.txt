set(models_MOC_HDRS
    braggReflectionFilterProxy.h
    braggReflectionModel.h
    dataManagerReferencePointModel.h
    dataManagerSimulationModel.h
    xmlAngleModel.h
    xmlAtomModel.h
    xmlBondModel.h
    xmlImproperModel.h
    xmlTreeModel.h
    xmlTorsionModel.h
    speciesAngleModel.h
    speciesAtomModel.h
    speciesBondModel.h
    speciesImproperModel.h
    speciesTorsionModel.h
)

qt5_wrap_cpp(models_MOC_SRCS ${models_MOC_HDRS})

set(models_SRCS
    braggReflectionFilterProxy.cpp
    braggReflectionModel.cpp
    dataManagerReferencePointModel.cpp
    dataManagerSimulationModel.cpp
    xmlAngleModel.cpp
    xmlAtomModel.cpp
    xmlBondModel.cpp
    xmlImproperModel.cpp
    xmlTreeModel.cpp
    xmlTorsionModel.cpp
    speciesAngleModel.cpp
    speciesAtomModel.cpp
    speciesBondModel.cpp
    speciesImproperModel.cpp
    speciesModelUtils.cpp
    speciesTorsionModel.cpp
)

qt_wrap_cpp(
  models_SRCS
  braggReflectionModel.h
  dataManagerReferencePointModel.h
  dataManagerSimulationModel.h
  xmlAngleModel.h
  xmlAtomModel.h
  xmlBondModel.h
  xmlImproperModel.h
  xmlTreeModel.h
  xmlTorsionModel.h
  speciesAngleModel.h
  speciesAtomModel.h
  speciesBondModel.h
  speciesImproperModel.h
  speciesTorsionModel.h
)

add_library(models ${models_SRCS} ${models_MOC_SRCS})

include_directories(models PUBLIC ${CONAN_INCLUDE_DIRS_PUGIXML} PRIVATE ${PROJECT_SOURCE_DIR}/src ${CONAN_INCLUDE_DIRS_FMT})

target_include_directories(
  models PRIVATE ${PROJECT_SOURCE_DIR}/src ${PROJECT_BINARY_DIR}/src ${Qt5Widgets_INCLUDE_DIRS} ${FREETYPE_INCLUDE_DIRS}
)
# target_link_libraries(models keywordwidgets charts delegates widgets components)

target_link_libraries(models PUBLIC CONAN_PKG::pugixml)

include_directories(models PRIVATE ${PROJECT_SOURCE_DIR}/src)

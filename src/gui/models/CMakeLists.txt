set(models_MOC_HDRS
    atomTypeModel.h
    braggReflectionFilterProxy.h
    braggReflectionModel.h
    dataManagerReferencePointModel.h
    dataManagerSimulationModel.h
    enumOptionsModel.h
    isotopologueSetModel.h
    pairPotentialModel.h
    procedureNodeModel.h
    masterTermModel.h
    sitesFilterProxy.h
    sitesModel.h
    speciesFilterProxy.h
    speciesModel.h
    speciesSiteFilterProxy.h
    speciesSiteModel.h
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
    speciesIsoModel.h
    speciesTorsionModel.h
)

qt5_wrap_cpp(models_MOC_SRCS ${models_MOC_HDRS})

set(models_SRCS
    atomTypeModel.cpp
    braggReflectionFilterProxy.cpp
    braggReflectionModel.cpp
    dataManagerReferencePointModel.cpp
    dataManagerSimulationModel.cpp
    enumOptionsModel.cpp
    isotopologueSetModel.cpp
    masterTermModel.cpp
    pairPotentialModel.cpp
    procedureNodeModel.cpp
    sitesFilterProxy.cpp
    sitesModel.cpp
    speciesAngleModel.cpp
    speciesAtomModel.cpp
    speciesBondModel.cpp
    speciesFilterProxy.cpp
    speciesImproperModel.cpp
    speciesIsoModel.cpp
    speciesModel.cpp
    speciesModelUtils.cpp
    speciesSiteFilterProxy.cpp
    speciesSiteModel.cpp
    speciesTorsionModel.cpp
    xmlAngleModel.cpp
    xmlAtomModel.cpp
    xmlBondModel.cpp
    xmlImproperModel.cpp
    xmlTreeModel.cpp
    xmlTorsionModel.cpp
)

qt_wrap_cpp(
  models_SRCS
  atomTypeModel.h
  braggReflectionModel.h
  dataManagerReferencePointModel.h
  dataManagerSimulationModel.h
  enumOptionsModel.h
  isotopologueSetModel.h
  pairPotentialModel.h
  speciesModel.h
  speciesSiteModel.h
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
  speciesIsoModel.h
  speciesTorsionModel.h
)

add_library(models ${models_SRCS} ${models_MOC_SRCS})

include_directories(models PUBLIC ${CONAN_INCLUDE_DIRS_PUGIXML} PRIVATE ${PROJECT_SOURCE_DIR}/src ${CONAN_INCLUDE_DIRS_FMT})

target_include_directories(
  models PRIVATE ${PROJECT_SOURCE_DIR}/src ${PROJECT_BINARY_DIR}/src ${Qt5Widgets_INCLUDE_DIRS} ${FREETYPE_INCLUDE_DIRS}
)

target_link_libraries(models PUBLIC CONAN_PKG::pugixml ${THREADING_LINK_LIBS})

include_directories(models PRIVATE ${PROJECT_SOURCE_DIR}/src)

set(models_MOC_HDRS
    atomTypeModel.h
    braggReflectionFilterProxy.h
    braggReflectionModel.h
    cifAssemblyModel.h
    configurationModel.h
    dataManagerReferencePointModel.h
    dataManagerSimulationModel.h
    enumOptionsModel.h
    isotopologueSetModel.h
    pairPotentialModel.h
    procedureNodeModel.h
    masterTermModel.h
    moduleLayerModel.h
    moduleModel.h
    modulePaletteModel.h
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

qt6_wrap_cpp(models_MOC_SRCS ${models_MOC_HDRS})

set(models_SRCS
    atomTypeFilterProxy.cpp
    atomTypeModel.cpp
    braggReflectionFilterProxy.cpp
    braggReflectionModel.cpp
    cifAssemblyModel.cpp
    configurationModel.cpp
    dataManagerReferencePointModel.cpp
    dataManagerSimulationModel.cpp
    enumOptionsModel.cpp
    expressionVariableVectorModel.cpp
    isotopologueSetModel.cpp
    masterTermModel.cpp
    moduleLayerModel.cpp
    moduleModel.cpp
    modulePaletteModel.cpp
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
  atomTypeFilterProxy.h
  atomTypeModel.h
  braggReflectionModel.h
  dataManagerReferencePointModel.h
  dataManagerSimulationModel.h
  enumOptionsModel.h
  expressionVariableVectorModel.h
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
  models PRIVATE ${PROJECT_SOURCE_DIR}/src ${PROJECT_BINARY_DIR}/src ${Qt6Widgets_INCLUDE_DIRS} ${FREETYPE_INCLUDE_DIRS}
)

if(CONAN)
  target_link_libraries(models PUBLIC CONAN_PKG::pugixml ${THREADING_LINK_LIBS})
else(CONAN)
  target_link_libraries(models PUBLIC pugixml::pugixml ${THREADING_LINK_LIBS})
endif(CONAN)

include_directories(models PRIVATE ${PROJECT_SOURCE_DIR}/src)

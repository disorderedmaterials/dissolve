add_subdirectory(nodeGraph)

set(models_MOC_HDRS
    addSpeciesInfoModel.h
    atomTypeModel.h
    braggReflectionFilterProxy.h
    braggReflectionModel.h
    configurationModel.h
    dissolveModel.h
    forcefieldModel.h
    isotopologueSetModel.h
    commonAngleModel.h
    commonBondModel.h
    commonImproperModel.h
    commonTermModel.h
    commonTermTreeModel.h
    commonTorsionModel.h
    modifyChargesModel.h
    nodePaletteModel.h
    pairPotentialModel.h
    pairPotentialOverrideModel.h
    generatorModel.h
    generatorNodeModel.h
    rangeVectorModel.h
    renderableGroupManagerModel.h
    simpleForcefieldModel.h
    sitesFilterProxy.h
    sitesModel.h
    sortFilterProxy.h
    speciesFilterProxy.h
    speciesModel.h
    speciesSiteFilterProxy.h
    speciesSiteModel.h
    speciesAngleModel.h
    speciesAtomModel.h
    speciesBondModel.h
    speciesImproperModel.h
    speciesIsoModel.h
    speciesTorsionModel.h
    xmlAngleModel.h
    xmlAtomModel.h
    xmlBondModel.h
    xmlImproperModel.h
    xmlTreeModel.h
    xmlTorsionModel.h
    modelUpdater.h
)

qt6_wrap_cpp(models_MOC_SRCS ${models_MOC_HDRS})

set(models_SRCS
    addSpeciesInfoModel.cpp
    atomTypeFilterProxy.cpp
    atomTypeModel.cpp
    braggReflectionFilterProxy.cpp
    braggReflectionModel.cpp
    configurationModel.cpp
    dissolveModel.cpp
    dissolveModelImageProvider.cpp
    dissolveModelImageProvider.h
    expressionVariableVectorModel.cpp
    externalPotentialModel.cpp
    ffSortFilterModel.cpp
    forcefieldModel.cpp
    globalPotentialFilterProxy.cpp
    isotopologueSetModel.cpp
    commonAngleModel.cpp
    commonBondModel.cpp
    commonImproperModel.cpp
    commonTermModel.cpp
    commonTermTreeModel.cpp
    commonTorsionModel.cpp
    modifyChargesModel.cpp
    nodePaletteModel.cpp
    pairPotentialModel.cpp
    pairPotentialOverrideModel.cpp
    generatorModel.cpp
    generatorModelMimeData.cpp
    generatorModelMimeData.h
    generatorNodeModel.cpp
    rangeVectorModel.cpp
    renderableGroupManagerModel.cpp
    simpleForcefieldModel.cpp
    sitesFilterProxy.cpp
    sitesModel.cpp
    sortFilterProxy.cpp
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
    modelUpdater.cpp
)

qt6_wrap_cpp(
  models_SRCS
  atomTypeFilterProxy.h
  atomTypeModel.h
  braggReflectionModel.h
  expressionVariableVectorModel.h
  forcefieldModel.h
  isotopologueSetModel.h
  pairPotentialModel.h
  rangeVectorModel.h
  simpleForcefieldModel.h
  sortFilterProxy.h
  speciesAngleModel.h
  speciesAtomModel.h
  speciesBondModel.h
  speciesImproperModel.h
  speciesIsoModel.h
  speciesModel.h
  speciesSiteModel.h
  speciesTorsionModel.h
  xmlAngleModel.h
  xmlAtomModel.h
  xmlBondModel.h
  xmlImproperModel.h
  xmlTreeModel.h
  xmlTorsionModel.h
)

set(# QML type register
    types_SRCS types.h types.cpp
)

find_package(Qt6 REQUIRED COMPONENTS Core Qml)

add_library(models ${models_SRCS} ${models_MOC_SRCS} ${types_SRCS})

target_include_directories(
  models PRIVATE ${PROJECT_SOURCE_DIR}/src ${PROJECT_BINARY_DIR}/src ${Qt6Widgets_INCLUDE_DIRS} ${FREETYPE_INCLUDE_DIRS}
                 ${Qt6Quick_INCLUDE_DIRS}
)

target_link_libraries(models PRIVATE Qt6::Core Qt6::Qml nodeGraphModel)

# Because Qt has its own way of checking for C++17
if(MSVC)
  target_compile_options(models PUBLIC /Zc:__cplusplus)
endif(MSVC)

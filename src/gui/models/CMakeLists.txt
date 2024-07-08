set(models_MOC_HDRS
    addForcefieldDialogModel.h
    addSpeciesInfoModel.h
    atomTypeModel.h
    braggReflectionFilterProxy.h
    braggReflectionModel.h
    cifAssemblyModel.h
    configurationModel.h
    dataManagerSimulationModel.h
    dissolveModel.h
    enumOptionsModel.h
    forcefieldModel.h
    isotopologueSetModel.h
    masterAngleModel.h
    masterBondModel.h
    masterImproperModel.h
    masterTermModel.h
    masterTermTreeModel.h
    masterTorsionModel.h
    modifyChargesModel.h
    moduleLayerModel.h
    moduleLayersModel.h
    moduleModel.h
    modulePaletteModel.h
    nodePaletteModel.h
    pairPotentialModel.h
    pairPotentialOverrideModel.h
    procedureModel.h
    procedureNodeModel.h
    rangeVectorModel.h
    renderableGroupManagerModel.h
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
    weightedModuleModel.h
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
    addForcefieldDialogModel.cpp
    addSpeciesInfoModel.cpp
    atomTypeFilterProxy.cpp
    atomTypeModel.cpp
    braggReflectionFilterProxy.cpp
    braggReflectionModel.cpp
    cifAssemblyModel.cpp
    configurationModel.cpp
    dataManagerSimulationModel.cpp
    dissolveModel.cpp
    dissolveModelImageProvider.cpp
    dissolveModelImageProvider.h
    enumOptionsModel.cpp
    expressionVariableVectorModel.cpp
    externalPotentialModel.cpp
    ffSortFilterModel.cpp
    forcefieldModel.cpp
    globalPotentialFilterProxy.cpp
    isotopologueSetModel.cpp
    masterAngleModel.cpp
    masterBondModel.cpp
    masterImproperModel.cpp
    masterTermModel.cpp
    masterTermTreeModel.cpp
    masterTorsionModel.cpp
    modifyChargesModel.cpp
    moduleLayerModel.cpp
    moduleLayersModel.cpp
    moduleModel.cpp
    modulePaletteModel.cpp
    nodePaletteModel.cpp
    pairPotentialModel.cpp
    pairPotentialOverrideModel.cpp
    procedureModel.cpp
    procedureModelMimeData.cpp
    procedureModelMimeData.h
    procedureNodeModel.cpp
    rangeVectorModel.cpp
    renderableGroupManagerModel.cpp
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
    weightedModuleModel.cpp
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
  dataManagerSimulationModel.h
  enumOptionsModel.h
  expressionVariableVectorModel.h
  forcefieldModel.h
  isotopologueSetModel.h
  pairPotentialModel.h
  rangeVectorModel.h
  sortFilterProxy.h
  speciesAngleModel.h
  speciesAtomModel.h
  speciesBondModel.h
  speciesImproperModel.h
  speciesIsoModel.h
  speciesModel.h
  speciesSiteModel.h
  speciesTorsionModel.h
  weightedModuleModel.h
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
  models PRIVATE ${PROJECT_SOURCE_DIR}/src ${PROJECT_BINARY_DIR}/src ${CONAN_INCLUDE_DIRS_PUGIXML} ${Qt6Widgets_INCLUDE_DIRS}
                 ${FREETYPE_INCLUDE_DIRS} ${Qt6Quick_INCLUDE_DIRS}
)

target_link_libraries(models PRIVATE Qt6::Core Qt6::Qml)

# Because Qt has its own way of checking for C++17
if(MSVC)
  target_compile_options(models PUBLIC /Zc:__cplusplus)
endif(MSVC)

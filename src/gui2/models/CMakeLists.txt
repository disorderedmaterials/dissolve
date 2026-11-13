# NodeGraph ###
set(nodeGraphModels_MOC_HDRS
    enumOptionsModel.h
    enumRegistry.h
    exampleGraphModel.h
    graphModelBase.h
    graphEdges.h
    graphNodeModel.h
    generatorGraphModel.h
    nodeWrapper.h
    nodeRegistryModel.h
    nodeRegistrySortFilterModel.h
    nodeMessages.h
    parameterEndPointsModel.h
)

set(nodeGraphModel_SRCS
    enumOptionsModel.cpp
    enumRegistry.cpp
    graphEdges.cpp
    graphModel.cpp
    graphNodeModel.cpp
    parameterModel.cpp
    nodeRegistryModel.cpp
    nodeRegistrySortFilterModel.cpp
    nodeMessages.cpp
    parameterEndPointsModel.cpp
)

# Plot ###
set(plotModels_MOC_HDRS plot/plot1DModel.h plot/renderableModel.h)

set(plotModels_SRCS plot/plot1DModel.cpp plot/renderableModel.cpp)

# QML Types ###
set(# QML type register
    types_SRCS types.h types.cpp
)

# Legacy ###
set(legacyModels_MOC_HDRS
    legacy/atomTypeModel.h
    legacy/configurationModel.h
    dissolveModel.h
    legacy/commonAngleModel.h
    legacy/commonBondModel.h
    legacy/commonImproperModel.h
    legacy/commonTermModel.h
    legacy/commonTermTreeModel.h
    legacy/commonTorsionModel.h
    legacy/modelUpdater.h
    legacy/speciesAtomModel.h
    legacy/speciesModel.h
    legacy/speciesBondModel.h
    legacy/speciesImproperModel.h
    legacy/speciesTorsionModel.h
    legacy/speciesAngleModel.h
    legacy/simpleForcefieldModel.h
)

qt6_wrap_cpp(models_MOC_SRCS ${models_MOC_HDRS})

set(legacyModels_SRCS
    legacy/atomTypeModel.cpp
    legacy/configurationModel.cpp
    dissolveModel.cpp
    legacy/commonAngleModel.cpp
    legacy/commonBondModel.cpp
    legacy/commonImproperModel.cpp
    legacy/commonTermModel.cpp
    legacy/commonTermTreeModel.cpp
    legacy/commonTorsionModel.cpp
    legacy/modelUpdater.cpp
    legacy/speciesAtomModel.cpp
    legacy/speciesModel.cpp
    legacy/speciesAngleModel.cpp
    legacy/speciesBondModel.cpp
    legacy/speciesImproperModel.cpp
    legacy/speciesTorsionModel.cpp
    legacy/simpleForcefieldModel.cpp
)

find_package(Qt6 REQUIRED COMPONENTS Core Qml)

add_library(
  models2
  ${nodeGraphModel_SRCS}
  ${nodeGraphModel_MOC_SRCS}
  ${types_SRCS}
  ${legacyModels_SRCS}
  ${legacyModels_MOC_SRCS}
  ${plotModels_SRCS}
  ${plotModels_MOC_SRCS}
)

target_include_directories(
  models2 PRIVATE ${PROJECT_SOURCE_DIR}/src ${PROJECT_BINARY_DIR}/src ${CONAN_INCLUDE_DIRS_PUGIXML} ${Qt6Widgets_INCLUDE_DIRS}
                  ${FREETYPE_INCLUDE_DIRS} ${Qt6Quick_INCLUDE_DIRS}
)

target_link_libraries(models2 PRIVATE Qt6::Core Qt6::Qml)

# Because Qt has its own way of checking for C++17
if(MSVC)
  target_compile_options(models2 PUBLIC /Zc:__cplusplus)
endif(MSVC)

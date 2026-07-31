# NodeGraph ###
set(nodeGraphModels_MOC_HDRS
    nodeGraph/enumOptionsModel.h
    nodeGraph/enumRegistry.h
    nodeGraph/exampleGraphModel.h
    nodeGraph/graphModelBase.h
    nodeGraph/graphEdgeModel.h
    nodeGraph/graphNodeModel.h
    nodeGraph/generatorGraphModel.h
    nodeGraph/nodeWrapper.h
    nodeGraph/nodeRegistryModel.h
)

set(nodeGraphModel_SRCS
    nodeGraph/enumOptionsModel.cpp
    nodeGraph/enumRegistry.cpp
    nodeGraph/graphEdgeModel.cpp
    nodeGraph/graphModel.cpp
    nodeGraph/graphNodeModel.cpp
    nodeGraph/parameterModel.cpp
    nodeGraph/nodeRegistryModel.cpp
)

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

add_library(models2 ${nodeGraphModel_SRCS} ${nodeGraphModel_MOC_SRCS} ${types_SRCS} ${legacyModels_SRCS} ${legacyModels_MOC_SRCS})

target_include_directories(
  models2 PRIVATE ${PROJECT_SOURCE_DIR}/src ${PROJECT_BINARY_DIR}/src ${CONAN_INCLUDE_DIRS_PUGIXML} ${Qt6Widgets_INCLUDE_DIRS}
                  ${FREETYPE_INCLUDE_DIRS} ${Qt6Quick_INCLUDE_DIRS}
)

target_link_libraries(models2 PRIVATE Qt6::Core Qt6::Qml)

# Because Qt has its own way of checking for C++17
if(MSVC)
  target_compile_options(models2 PUBLIC /Zc:__cplusplus)
endif(MSVC)

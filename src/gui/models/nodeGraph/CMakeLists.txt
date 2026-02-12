set(nodeGraphModels_MOC_HDRS
    enumOptionsModel.h
    enumRegistry.h
    exampleGraphModel.h
    graphModelBase.h
    graphEdgeModel.h
    graphNodeModel.h
    generatorGraphModel.h
    nodeWrapper.h
)

set(nodeGraphModel_SRCS enumOptionsModel.cpp enumRegistry.cpp graphEdgeModel.cpp graphModel.cpp graphNodeModel.cpp parameterModel.cpp)

find_package(Qt6 REQUIRED COMPONENTS Core Qml)

add_library(nodeGraphModel ${nodeGraphModel_SRCS} ${nodeGraphModel_MOC_SRCS} ${types_SRCS})

target_include_directories(
  nodeGraphModel PRIVATE ${PROJECT_SOURCE_DIR}/src ${PROJECT_BINARY_DIR}/src ${CONAN_INCLUDE_DIRS_PUGIXML} ${Qt6Widgets_INCLUDE_DIRS}
                         ${FREETYPE_INCLUDE_DIRS} ${Qt6Quick_INCLUDE_DIRS}
)

target_link_libraries(nodeGraphModel PRIVATE Qt6::Core Qt6::Qml)

# Because Qt has its own way of checking for C++17
if(MSVC)
  target_compile_options(nodeGraphModel PUBLIC /Zc:__cplusplus)
endif(MSVC)

set(nodeGraphModels_MOC_HDRS graphEdgeModel.h graphNodeModel.h nodeWrapper.h)

qt6_wrap_cpp(nodeGraphModel_MOC_SRCS ${nodeGraphModel_MOC_HDRS})

set(nodeGraphModel_SRCS graphModel.cpp nodeWrapper.cpp)

qt6_wrap_cpp(nodeGraphModel_SRCS graphEdgeModel.h graphNodeModel.h nodeWrapper.h)

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

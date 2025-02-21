qt_add_library(graphNode_module STATIC)

qt_add_qml_module(graphNode_module
	URI
	"GraphNodeModule"
	VERSION
	1.0
	SOURCES
	base.h
	parameter.h
	node.h
	nodes/module.h
	nodes/generator.h
	nodes/vec3Decomposition.h
	base.cpp
	node.cpp
)

find_package(Qt6 REQUIRED COMPONENTS Core)
target_link_libraries(graphNode_module PRIVATE Qt6::Core)
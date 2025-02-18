qt_add_library(icons_module)

file(GLOB basic_icons RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "${CMAKE_CURRENT_SOURCE_DIR}/*.svg")
file(GLOB node_icons RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "${CMAKE_CURRENT_SOURCE_DIR}/nodes/*.svg")
file(GLOB module_icons RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "${CMAKE_CURRENT_SOURCE_DIR}/modules/*.svg")

qt_add_qml_module(
	icons_module
	URI
	"IconsModule"
	RESOURCES
		${basic_icons}
		${node_icons}
		${module_icons}
)
qt_add_library(icons_module STATIC)

file(
  GLOB basic_icons
  RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
  "./*.svg"
)
file(
  GLOB node_icons
  RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
  "./nodes/*.svg"
)
file(
  GLOB module_icons
  RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
  "./modules/*.svg"
)

qt_add_qml_module(
  icons_module
  URI
  "DissolveIconsModule"
  VERSION
  1.0
  RESOURCES
  ${basic_icons}
  ${node_icons}
  ${module_icons}
)

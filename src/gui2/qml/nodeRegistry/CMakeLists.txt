qt_add_library(nodeRegistry_module STATIC)

qt6_add_qml_module(
  nodeRegistry_module
  URI
  "NodeRegistryModule"
  VERSION
  1.0
  QML_FILES
  NodeSearchDialog.qml
  NodeDisplayDelegate.qml
)

find_package(Qt6 REQUIRED COMPONENTS Core)
target_link_libraries(nodeRegistry_module PRIVATE Qt6::Core)

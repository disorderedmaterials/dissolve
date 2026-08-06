qt_add_library(nodeGraph_module STATIC)

qt_add_qml_module(
  nodeGraph_module
  VERSION
  0.1
  URI
  "DissolveNodeGraphModule"
  QML_FILES
  ForcefieldDialog.qml
  GraphView.qml
  GraphDelegate.qml
  ParameterDelegate.qml
  NodeBox.qml
  SpeciesDialog.qml
  RESOURCES
)

find_package(Qt6 REQUIRED COMPONENTS Core)
target_link_libraries(nodeGraph_module PRIVATE Qt6::Core)

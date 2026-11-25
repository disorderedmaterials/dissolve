qt_add_library(configuration_viewer STATIC)

qt6_add_qml_module(
  configuration_viewer
  URI
  "DissolveConfigurationViewer"
  VERSION
  1.0
  QML_FILES
  DissolveConfigurationViewer.qml
)

find_package(Qt6 REQUIRED COMPONENTS Core)
target_link_libraries(configuration_viewer PRIVATE Qt6::Core)

qt_add_library(controls_module STATIC)

qt6_add_qml_module(
  controls_module
  URI
  "DissolveControlsModule"
  VERSION
  1.0
  QML_FILES
  DissolveMenuItem.qml
)

find_package(Qt6 REQUIRED COMPONENTS Core)
target_link_libraries(controls_module PRIVATE Qt6::Core)

qt_add_library(periodicTable_module STATIC)

qt_add_qml_module(
  periodicTable_module
  VERSION
  0.1
  URI
  "DissolvePeriodicTableModule"
  QML_FILES
  ElementComboBox.qml
  RESOURCES
)

find_package(Qt6 REQUIRED COMPONENTS Core)
target_link_libraries(periodicTable_module PRIVATE Qt6::Core)

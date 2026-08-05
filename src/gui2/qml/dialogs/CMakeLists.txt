qt_add_library(dialogs_module STATIC)

qt6_add_qml_module(
  dialogs_module
  URI
  "DissolveDialogsModule"
  VERSION
  1.0
  QML_FILES
  NodeSearchDialog.qml
  NodeDisplayDelegate.qml
)

find_package(Qt6 REQUIRED COMPONENTS Core)
target_link_libraries(dialogs_module PRIVATE Qt6::Core)

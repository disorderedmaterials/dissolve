qt_add_library(delegates_module STATIC)

qt6_add_qml_module(
  delegates_module
  URI
  "DissolveDelegatesModule"
  VERSION
  1.0
  QML_FILES
  RangedVector3Delegate.qml
)

find_package(Qt6 REQUIRED COMPONENTS Core)
target_link_libraries(delegates_module PRIVATE Qt6::Core)

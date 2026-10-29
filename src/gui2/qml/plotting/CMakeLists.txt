qt_add_library(plotting_module STATIC)

qt6_add_qml_module(
  plotting_module
  URI
  "DissolvePlottingModule"
  VERSION
  1.0
  QML_FILES
  Plot1DContext.qml
  Plot1DCanvas.qml
)

find_package(Qt6 REQUIRED COMPONENTS Core Quick)
target_link_libraries(plotting_module PRIVATE Qt6::Core Qt6::Quick)

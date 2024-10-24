# Target 'keywordWidgets'
add_library(
  keywordWidgets
  atomTypeVector.cpp
  atomTypeVector.h
  atomTypeVector.ui
  base.cpp
  base.h
  bool.cpp
  bool.h
  CMakeLists.txt
  configuration.cpp
  configuration.h
  configuration.ui
  configurationVector.cpp
  configurationVector.h
  configurationVector.ui
  dialog.cpp
  dialog.h
  dialog.ui
  double.cpp
  double.h
  dropDown.cpp
  dropDown.h
  dropDown.ui
  dropWidget.cpp
  dropWidget.h
  enumOptions.cpp
  enumOptions.h
  expressionVariableVector.cpp
  expressionVariableVector.h
  expressionVariableVector.ui
  fileAndFormat.cpp
  fileAndFormat.h
  fileAndFormat.ui
  function1D.cpp
  function1D.h
  function1D.ui
  integer.cpp
  integer.h
  interactionPotential.cpp
  interactionPotential.h
  interactionPotential.ui
  isotopologueSet.cpp
  isotopologueSet.h
  isotopologueSet.ui
  layer.cpp
  layer.h
  layer.ui
  module.cpp
  module.h
  module.ui
  moduleVector.cpp
  moduleVector.h
  moduleVector.ui
  node.cpp
  node.h
  node.ui
  nodeValue.cpp
  nodeValueEnumOptions.cpp
  nodeValueEnumOptions.h
  nodeValueEnumOptions.ui
  nodeValue.h
  nodeValue.ui
  nodeVector.cpp
  nodeVector.h
  nodeVector.ui
  optionalDouble.cpp
  optionalDouble.h
  optionalInt.cpp
  optionalInt.h
  producers.cpp
  producers.h
  range.cpp
  range.h
  range.ui
  rangeVector.cpp
  rangeVector.h
  rangeVector.ui
  species.cpp
  species.h
  speciesSite.cpp
  speciesSite.h
  speciesSite.ui
  speciesSiteVector.cpp
  speciesSiteVector.h
  speciesSiteVector.ui
  species.ui
  speciesVector.cpp
  speciesVector.h
  speciesVector.ui
  stdString.cpp
  stdString.h
  vec3Double.cpp
  vec3Double.h
  vec3Double.ui
  vec3Integer.cpp
  vec3Integer.h
  vec3Integer.ui
  vec3Labels.cpp
  vec3Labels.h
  vec3NodeValue.cpp
  vec3NodeValue.h
  vec3NodeValue.ui
  weightedModuleVector.cpp
  weightedModuleVector.h
  weightedModuleVector.ui
  widget.cpp
  widgetGroupHeader.cpp
  widgetGroupHeader.h
  widgetGroupHeader.ui
  widget.h
)
target_include_directories(keywordWidgets PRIVATE ${PROJECT_SOURCE_DIR}/src ${PROJECT_BINARY_DIR}/src ${Qt6Widgets_INCLUDE_DIRS})
target_link_libraries(keywordWidgets PRIVATE base)

# Because Qt has its own way of checking for C++17
if(MSVC)
  target_compile_options(keywordWidgets PUBLIC /Zc:__cplusplus)
endif(MSVC)

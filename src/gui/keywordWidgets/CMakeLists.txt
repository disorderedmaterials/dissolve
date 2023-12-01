# Don't forget to include output directory, otherwise the UI file won't be wrapped!

# Meta-Objects
set(keywordWidgets_MOC_HDRS
    atomTypeVector.h
    configuration.h
    configurationVector.h
    dialog.h
    dropDown.h
    expressionVariableVector.h
    fileAndFormat.h
    function1D.h
    interactionPotential.h
    isotopologueSet.h
    layer.h
    module.h
    moduleVector.h
    node.h
    nodeAndInteger.h
    nodeValue.h
    nodeValueEnumOptions.h
    nodeVector.h
    range.h
    species.h
    speciesSite.h
    speciesSiteVector.h
    speciesVector.h
    vec3Double.h
    vec3Integer.h
    vec3NodeValue.h
    weightedModuleVector.h
    rangeVector.h
    bool.hui
    double.hui
    dropWidget.hui
    enumOptions.hui
    integer.hui
    optionalDouble.hui
    optionalInt.hui
    stdString.hui
    widget.hui
    widgetGroupHeader.h
)
qt6_wrap_cpp(keywordWidgets_MOC_SRCS ${keywordWidgets_MOC_HDRS})

# User Interface Files
set(keywordWidgets_UIS
    atomTypeVector.ui
    configuration.ui
    configurationVector.ui
    dialog.ui
    dropDown.ui
    expressionVariableVector.ui
    fileAndFormat.ui
    function1D.ui
    interactionPotential.ui
    isotopologueSet.ui
    layer.ui
    module.ui
    moduleVector.ui
    node.ui
    nodeAndInteger.ui
    nodeValue.ui
    nodeValueEnumOptions.ui
    nodeVector.ui
    range.ui
    rangeVector.ui
    species.ui
    speciesSite.ui
    speciesSiteVector.ui
    speciesVector.ui
    vec3Double.ui
    vec3Integer.ui
    vec3NodeValue.ui
    weightedModuleVector.ui
    widgetGroupHeader.ui
)
qt6_wrap_ui(keywordWidgets_UIS_H ${keywordWidgets_UIS})

# Source files (not Qt UI files or custom .h [uih])
set(keywordWidgets_SRCS
    base.cpp
    atomTypeVectorFuncs.cpp
    boolFuncs.cpp
    configurationFuncs.cpp
    configurationVectorFuncs.cpp
    dialogFuncs.cpp
    doubleFuncs.cpp
    dropDownFuncs.cpp
    dropWidgetFuncs.cpp
    enumOptionsFuncs.cpp
    expressionVariableVectorFuncs.cpp
    fileAndFormatFuncs.cpp
    function1DFuncs.cpp
    integerFuncs.cpp
    interactionPotentialFuncs.cpp
    isotopologueSetFuncs.cpp
    layerFuncs.cpp
    moduleFuncs.cpp
    moduleVectorFuncs.cpp
    nodeFuncs.cpp
    nodeAndIntegerFuncs.cpp
    nodeValueFuncs.cpp
    nodeValueEnumOptionsFuncs.cpp
    nodeVectorFuncs.cpp
    optionalDoubleFuncs.cpp
    optionalIntFuncs.cpp
    producers.cpp
    rangeFuncs.cpp
    rangeVectorFuncs.cpp
    speciesFuncs.cpp
    speciesSiteFuncs.cpp
    speciesSiteVectorFuncs.cpp
    speciesVectorFuncs.cpp
    stdStringFuncs.cpp
    vec3DoubleFuncs.cpp
    vec3IntegerFuncs.cpp
    vec3Labels.cpp
    vec3NodeValueFuncs.cpp
    weightedModuleVectorFuncs.cpp
    widgetFuncs.cpp
    widgetGroupHeaderFuncs.cpp
    base.h
    vec3Labels.h
    producers.h
)

# Target 'keywordWidgets'
add_library(keywordWidgets ${keywordWidgets_UIS_H} ${keywordWidgets_SRCS} ${keywordWidgets_MOC_SRCS})
target_include_directories(keywordWidgets PRIVATE ${PROJECT_SOURCE_DIR}/src ${PROJECT_BINARY_DIR}/src ${Qt6Widgets_INCLUDE_DIRS})
target_link_libraries(keywordWidgets PRIVATE base)

# Because Qt has its own way of checking for C++17
if(MSVC)
  target_compile_options(keywordWidgets PUBLIC /Zc:__cplusplus)
endif(MSVC)

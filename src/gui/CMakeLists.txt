# Don't forget to include output directory, otherwise the UI file won't be wrapped!

# -----------------
# Main Dissolve GUI
# -----------------

# Meta-Objects
set(gui_MOC_HDRS
    # Viewers
    configurationViewerWidget.h
    dataWidget.h
    selectSymbol.h
    speciesEditor.h
    speciesWidget.h
    configurationViewer.hui
    dataViewer.hui
    speciesViewer.hui
    viewer.hui
    # Widgets and Wizards
    moduleControlWidget.h
    nodeControlWidget.h
    procedureWidget.h
    selectAtomTypeWidget.h
    selectForcefieldWidget.h
    selectSpeciesWidget.h
    wizard.hui
    # Dialogs
    addConfigurationDialog.h
    addForcefieldTermsDialog.h
    copySpeciesTermsDialog.h
    dataManagerDialog.h
    editSpeciesDialog.h
    getConfigurationNameDialog.h
    getModuleLayerNameDialog.h
    getSpeciesNameDialog.h
    getTabNameDialog.h
    importLigParGenDialog.h
    importCIFDialog.h
    importSpeciesDialog.h
    intramolecularTermsDialog.h
    modifyChargesDialog.h
    selectAtomTypeDialog.h
    selectElementDialog.h
    selectGenericItemDialog.h
    selectRestartFileDialog.h
    selectSpeciesDialog.h
    # Main Tabs
    configurationTab.h
    forcefieldTab.h
    layerTab.h
    messagesTab.h
    overviewTab.h
    speciesTab.h
    # Main UI
    gui.h
    outputHandler.hui
    thread.hui
    mainTabsBar.hui
    mainTabsWidget.hui
    # QML type register
    types.h
)
qt6_wrap_cpp(gui_MOC_SRCS ${gui_MOC_HDRS})

# User Interface Files
set(gui_UIS
    # Viewers
    configurationViewerWidget.ui
    dataWidget.ui
    selectSymbol.ui
    speciesEditor.ui
    speciesWidget.ui
    # Widgets and Wizards
    moduleControlWidget.ui
    nodeControlWidget.ui
    procedureWidget.ui
    selectAtomTypeWidget.ui
    selectForcefieldWidget.ui
    selectSpeciesWidget.ui
    wizardFooter.ui
    wizardHeader.ui
    # Dialogs
    addConfigurationDialog.ui
    copySpeciesTermsDialog.ui
    dataManagerDialog.ui
    editSpeciesDialog.ui
    getConfigurationNameDialog.ui
    getModuleLayerNameDialog.ui
    getSpeciesNameDialog.ui
    getTabNameDialog.ui
    importLigParGenDialog.ui
    importCIFDialog.ui
    importSpeciesDialog.ui
    intramolecularTermsDialog.ui
    selectAtomTypeDialog.ui
    selectElementDialog.ui
    selectGenericItemDialog.ui
    selectRestartFileDialog.ui
    selectSpeciesDialog.ui
    # Main Tabs
    configurationTab.ui
    forcefieldTab.ui
    layerTab.ui
    messagesTab.ui
    speciesTab.ui
    # Main UI
    gui.ui
)
qt6_wrap_ui(gui_UIS_H ${gui_UIS})

# Source files (not Qt UI files or custom .h [uih])
set(gui_SRCS
    # Viewers
    configurationViewerFuncs.cpp
    configurationViewer_input.cpp
    configurationViewer_interaction.cpp
    configurationViewer_render.cpp
    configurationViewerWidgetFuncs.cpp
    dataViewerFuncs.cpp
    dataViewer_input.cpp
    dataViewer_interaction.cpp
    dataViewer_contextMenu.cpp
    dataViewer_render.cpp
    dataWidgetFuncs.cpp
    selectSymbolFuncs.cpp
    speciesEditorFuncs.cpp
    speciesViewerFuncs.cpp
    speciesViewer_input.cpp
    speciesViewer_interaction.cpp
    speciesViewer_render.cpp
    speciesWidgetFuncs.cpp
    viewerFuncs.cpp
    viewer_input.cpp
    viewer_interaction.cpp
    viewer_options.cpp
    viewer_query.cpp
    viewer_render.cpp
    viewer_renderables.cpp
    viewer_view.cpp
    # Widgets and Wizards
    moduleControlWidgetFuncs.cpp
    nodeControlWidgetFuncs.cpp
    procedureWidgetFuncs.cpp
    selectAtomTypeWidgetFuncs.cpp
    selectForcefieldWidgetFuncs.cpp
    selectSpeciesWidgetFuncs.cpp
    wizardFuncs.cpp
    # Dialogs
    addConfigurationDialogFuncs.cpp
    addForcefieldTermsDialogFuncs.cpp
    copySpeciesTermsDialogFuncs.cpp
    dataManagerDialogFuncs.cpp
    editSpeciesDialogFuncs.cpp
    getConfigurationNameDialogFuncs.cpp
    getModuleLayerNameDialogFuncs.cpp
    getSpeciesNameDialogFuncs.cpp
    getTabNameDialogFuncs.cpp
    importLigParGenDialogFuncs.cpp
    importCIFDialogFuncs.cpp
    importSpeciesDialogFuncs.cpp
    intramolecularTermsDialogFuncs.cpp
    modifyChargesDialog.cpp
    selectAtomTypeDialogFuncs.cpp
    selectElementDialogFuncs.cpp
    selectGenericItemDialogFuncs.cpp
    selectRestartFileDialogFuncs.cpp
    selectSpeciesDialogFuncs.cpp
    # Main Tabs
    configurationTabFuncs.cpp
    forcefieldTabFuncs.cpp
    layerTabFuncs.cpp
    mainTab.cpp
    mainTabsBarFuncs.cpp
    mainTabsWidgetFuncs.cpp
    messagesTabFuncs.cpp
    overviewTabFuncs.cpp
    speciesTabFuncs.cpp
    speciesTab_geometry.cpp
    speciesTab_isotopologues.cpp
    speciesTab_sites.cpp
    mainTab.h
    # Main UI
    guiFuncs.cpp
    gui_simulation.cpp
    menu_configuration.cpp
    menu_file.cpp
    menu_help.cpp
    menu_layer.cpp
    menu_simulation.cpp
    menu_species.cpp
    outputHandlerFuncs.cpp
    stockColours.cpp
    threadFuncs.cpp
    stockColours.h
    # QML type register
    typesFuncs.cpp
)

# Resources
set(gui_RES_QRC main.qrc)
qt6_add_resources(gui_RES ${gui_RES_QRC})

# Target 'gui'
add_library(gui ${gui_RES} ${gui_UIS_H} ${gui_SRCS} ${gui_MOC_SRCS})
target_include_directories(
  gui
  PRIVATE ${PROJECT_SOURCE_DIR}/src
          ${PROJECT_BINARY_DIR}/src
          ${Qt6Widgets_INCLUDE_DIRS}
          ${FREETYPE_INCLUDE_DIRS}
          Qt6::OpenGL
          Qt6::OpenGLWidgets
          Qt6::Core
)

target_link_libraries(
  gui
  PRIVATE keywordWidgets
          delegates
          widgets
          models
          base
          Qt6::OpenGL
          Qt6::OpenGLWidgets
          Qt6::Quick
          Qt6::QuickControls2
          Qt6::Core
          Qt6::Gui
          Qt6::Qml
          Qt6::QmlModels
          Qt6::QuickWidgets
)

# Subdirectories
add_subdirectory(keywordWidgets)
add_subdirectory(delegates)
add_subdirectory(render)
add_subdirectory(widgets)
add_subdirectory(models)

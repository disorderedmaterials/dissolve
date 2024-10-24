# Target 'gui'
add_library(
  gui
  # Qt Resources
  main.qrc
  # Core Classes
  wizard.cpp
  wizard.h
  wizardFooter.h
  wizardFooter.ui
  wizardHeader.h
  wizardHeader.ui
  # Viewers
  configurationViewer.cpp
  configurationViewer_input.cpp
  configurationViewer_interaction.cpp
  configurationViewer_render.cpp
  configurationViewer.h
  configurationViewerWidget.cpp
  configurationViewerWidget.h
  configurationViewerWidget.ui
  dataViewer.cpp
  dataViewer_input.cpp
  dataViewer_interaction.cpp
  dataViewer_contextMenu.cpp
  dataViewer_render.cpp
  dataViewer.h
  dataWidget.cpp
  dataWidget.h
  dataWidget.ui
  selectSymbol.cpp
  selectSymbol.h
  selectSymbol.ui
  speciesEditor.cpp
  speciesEditor.h
  speciesEditor.ui
  speciesViewer.h
  speciesViewer.cpp
  speciesViewer_input.cpp
  speciesViewer_interaction.cpp
  speciesViewer_render.cpp
  speciesWidget.cpp
  speciesWidget.h
  speciesWidget.ui
  viewer.cpp
  viewer_input.cpp
  viewer_interaction.cpp
  viewer_options.cpp
  viewer_query.cpp
  viewer_render.cpp
  viewer_renderables.cpp
  viewer_view.cpp
  viewer.h
  # Widgets and Wizards
  moduleControlWidget.cpp
  moduleControlWidget.h
  moduleControlWidget.ui
  nodeControlWidget.cpp
  nodeControlWidget.h
  nodeControlWidget.ui
  generatorWidget.cpp
  generatorWidget.h
  generatorWidget.ui
  selectAtomTypeWidget.cpp
  selectAtomTypeWidget.h
  selectAtomTypeWidget.ui
  selectForcefieldWidget.cpp
  selectForcefieldWidget.h
  selectForcefieldWidget.ui
  selectSpeciesWidget.cpp
  selectSpeciesWidget.h
  selectSpeciesWidget.ui
  # Dialogs
  addConfigurationDialog.cpp
  addConfigurationDialog.h
  addConfigurationDialog.ui
  addForcefieldTermsDialog.cpp
  addForcefieldTermsDialog.h
  copySpeciesTermsDialog.cpp
  copySpeciesTermsDialog.h
  copySpeciesTermsDialog.ui
  dataManagerDialog.cpp
  dataManagerDialog.h
  editSpeciesDialog.cpp
  editSpeciesDialog.h
  editSpeciesDialog.ui
  getConfigurationNameDialog.cpp
  getConfigurationNameDialog.h
  getConfigurationNameDialog.ui
  getModuleLayerNameDialog.cpp
  getModuleLayerNameDialog.h
  getModuleLayerNameDialog.ui
  getSpeciesNameDialog.cpp
  getSpeciesNameDialog.h
  getSpeciesNameDialog.ui
  getTabNameDialog.cpp
  getTabNameDialog.h
  getTabNameDialog.ui
  importCIFDialog.cpp
  importCIFDialog.h
  importCIFDialog.ui
  importLigParGenDialog.cpp
  importLigParGenDialog.h
  importLigParGenDialog.ui
  importSpeciesDialog.cpp
  importSpeciesDialog.h
  importSpeciesDialog.ui
  intramolecularTermsDialog.cpp
  intramolecularTermsDialog.h
  intramolecularTermsDialog.ui
  modifyChargesDialog.cpp
  modifyChargesDialog.h
  selectAtomTypeDialog.cpp
  selectAtomTypeDialog.h
  selectAtomTypeDialog.ui
  selectElementDialog.cpp
  selectElementDialog.h
  selectElementDialog.ui
  selectGenericItemDialog.cpp
  selectGenericItemDialog.h
  selectGenericItemDialog.ui
  selectRestartFileDialog.cpp
  selectRestartFileDialog.h
  selectRestartFileDialog.ui
  selectSpeciesDialog.cpp
  selectSpeciesDialog.h
  selectSpeciesDialog.ui
  # Main Tabs
  configurationTab.cpp
  configurationTab.h
  configurationTab.ui
  forcefieldTab.cpp
  forcefieldTab.h
  forcefieldTab.ui
  layerTab.cpp
  layerTab.h
  layerTab.ui
  mainTab.cpp
  mainTab.h
  mainTabsBar.cpp
  mainTabsBar.h
  mainTabsWidget.cpp
  mainTabsWidget.h
  messagesTab.cpp
  messagesTab.h
  messagesTab.ui
  overviewTab.cpp
  overviewTab.h
  speciesTab.cpp
  speciesTab_geometry.cpp
  speciesTab_isotopologues.cpp
  speciesTab_sites.cpp
  speciesTab.h
  speciesTab.ui
  # Main UI
  gui.cpp
  gui_simulation.cpp
  gui.h
  gui.ui
  menu_configuration.cpp
  menu_file.cpp
  menu_help.cpp
  menu_layer.cpp
  menu_simulation.cpp
  menu_species.cpp
  outputHandler.cpp
  outputHandler.h
  stockColours.cpp
  stockColours.h
  thread.cpp
  thread.h
)

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

# Don't forget to include output directory, otherwise the UI file won't be wrapped!

# -----------------
# Main Dissolve GUI
# -----------------

# Meta-Objects
set(gui_MOC_HDRS
    # Viewers
    configurationviewerwidget.h
    datawidget.h
    selectsymbol.h
    sitewidget.h
    specieseditor.h
    specieswidget.h
    configurationviewer.hui
    dataviewer.hui
    siteviewer.hui
    speciesviewer.hui
    viewer.hui
    # Widgets and Wizards
    modulecontrolwidget.h
    nodecontrolwidget.h
    procedurewidget.h
    selectatomtypewidget.h
    selectconfigurationwidget.h
    selectforcefieldwidget.h
    selectspecieswidget.h
    wizard.hui
    # Dialogs
    addconfigurationdialog.h
    addforcefieldtermsdialog.h
    copyspeciestermsdialog.h
    datamanagerdialog.h
    editspeciesdialog.h
    getconfigurationnamedialog.h
    getmodulelayernamedialog.h
    getspeciesnamedialog.h
    gettabnamedialog.h
    importligpargendialog.h
    importcifdialog.h
    importspeciesdialog.h
    selectatomtypedialog.h
    selectconfigurationdialog.h
    selectelementdialog.h
    selectgenericitemdialog.h
    selectrestartfiledialog.h
    selectspeciesdialog.h
    # Main Tabs
    configurationtab.h
    forcefieldtab.h
    layertab.h
    messagestab.h
    speciestab.h
    # Main UI
    gui.h
    outputhandler.hui
    thread.hui
    maintabsbar.hui
    maintabswidget.hui
)
qt6_wrap_cpp(gui_MOC_SRCS ${gui_MOC_HDRS})

# User Interface Files
set(gui_UIS
    # Viewers
    configurationviewerwidget.ui
    datawidget.ui
    selectsymbol.ui
    sitewidget.ui
    specieseditor.ui
    specieswidget.ui
    # Widgets and Wizards
    modulecontrolwidget.ui
    nodecontrolwidget.ui
    procedurewidget.ui
    selectatomtypewidget.ui
    selectconfigurationwidget.ui
    selectforcefieldwidget.ui
    selectspecieswidget.ui
    wizardfooter.ui
    wizardheader.ui
    # Dialogs
    addconfigurationdialog.ui
    addforcefieldtermsdialog.ui
    copyspeciestermsdialog.ui
    datamanagerdialog.ui
    editspeciesdialog.ui
    getconfigurationnamedialog.ui
    getmodulelayernamedialog.ui
    getspeciesnamedialog.ui
    gettabnamedialog.ui
    importligpargendialog.ui
    importcifdialog.ui
    importspeciesdialog.ui
    selectatomtypedialog.ui
    selectconfigurationdialog.ui
    selectelementdialog.ui
    selectgenericitemdialog.ui
    selectrestartfiledialog.ui
    selectspeciesdialog.ui
    # Main Tabs
    configurationtab.ui
    forcefieldtab.ui
    layertab.ui
    messagestab.ui
    speciestab.ui
    # Main UI
    gui.ui
)
qt6_wrap_ui(gui_UIS_H ${gui_UIS})

# Source files (not Qt UI files or custom .h [uih])
set(gui_SRCS
    # Viewers
    configurationviewer_funcs.cpp
    configurationviewer_input.cpp
    configurationviewer_interaction.cpp
    configurationviewer_render.cpp
    configurationviewerwidget_funcs.cpp
    dataviewer_funcs.cpp
    dataviewer_input.cpp
    dataviewer_interaction.cpp
    dataviewer_contextmenu.cpp
    dataviewer_render.cpp
    datawidget_funcs.cpp
    selectsymbol_funcs.cpp
    siteviewer_funcs.cpp
    siteviewer_input.cpp
    siteviewer_interaction.cpp
    siteviewer_render.cpp
    sitewidget_funcs.cpp
    specieseditor_funcs.cpp
    speciesviewer_funcs.cpp
    speciesviewer_input.cpp
    speciesviewer_interaction.cpp
    speciesviewer_render.cpp
    specieswidget_funcs.cpp
    viewer_funcs.cpp
    viewer_input.cpp
    viewer_interaction.cpp
    viewer_options.cpp
    viewer_query.cpp
    viewer_render.cpp
    viewer_renderables.cpp
    viewer_view.cpp
    # Widgets and Wizards
    modulecontrolwidget_funcs.cpp
    nodecontrolwidget_funcs.cpp
    procedurewidget_funcs.cpp
    selectatomtypewidget_funcs.cpp
    selectconfigurationwidget_funcs.cpp
    selectforcefieldwidget_funcs.cpp
    selectspecieswidget_funcs.cpp
    wizard_funcs.cpp
    # Dialogs
    addconfigurationdialog_funcs.cpp
    addforcefieldtermsdialog_funcs.cpp
    copyspeciestermsdialog_funcs.cpp
    datamanagerdialog_funcs.cpp
    editspeciesdialog_funcs.cpp
    getconfigurationnamedialog_funcs.cpp
    getmodulelayernamedialog_funcs.cpp
    getspeciesnamedialog_funcs.cpp
    gettabnamedialog_funcs.cpp
    importligpargendialog_funcs.cpp
    importcifdialog_funcs.cpp
    importspeciesdialog_funcs.cpp
    selectatomtypedialog_funcs.cpp
    selectconfigurationdialog_funcs.cpp
    selectelementdialog_funcs.cpp
    selectgenericitemdialog_funcs.cpp
    selectrestartfiledialog_funcs.cpp
    selectspeciesdialog_funcs.cpp
    # Main Tabs
    configurationtab_funcs.cpp
    forcefieldtab_funcs.cpp
    layertab_funcs.cpp
    maintab.cpp
    maintabsbar_funcs.cpp
    maintabswidget_funcs.cpp
    messagestab_funcs.cpp
    speciestab_funcs.cpp
    speciestab_geometry.cpp
    speciestab_isotopologues.cpp
    speciestab_sites.cpp
    maintab.h
    # Main UI
    gui_funcs.cpp
    gui_simulation.cpp
    menu_configuration.cpp
    menu_file.cpp
    menu_help.cpp
    menu_layer.cpp
    menu_simulation.cpp
    menu_species.cpp
    outputhandler_funcs.cpp
    stockcolours.cpp
    thread_funcs.cpp
    stockcolours.h
)

# Resources
set(gui_RES_QRC main.qrc)
qt6_add_resources(gui_RES ${gui_RES_QRC})

# Target 'gui'
add_library(gui ${gui_RES} ${gui_UIS_H} ${gui_SRCS} ${gui_MOC_SRCS})
target_include_directories(
  gui PRIVATE ${PROJECT_SOURCE_DIR}/src ${PROJECT_BINARY_DIR}/src ${Qt6Widgets_INCLUDE_DIRS} ${FREETYPE_INCLUDE_DIRS} Qt6::OpenGL
              Qt6::OpenGLWidgets
)
target_link_libraries(
  gui
  keywordwidgets
  delegates
  widgets
  models
  base
  Qt6::OpenGL
  Qt6::OpenGLWidgets
)

# Subdirectories
add_subdirectory(keywordwidgets)
add_subdirectory(delegates)
add_subdirectory(render)
add_subdirectory(widgets)
add_subdirectory(models)

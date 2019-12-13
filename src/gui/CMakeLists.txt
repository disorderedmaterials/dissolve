# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!

# -----------
# Sub-Windows
# -----------

# Meta-Objects
SET(subwindows_MOC_HDRS
  # Widgets
  addconfigurationwizard.h
  addforcefieldtermswizard.h
  importspecieswizard.h
  selectconfigurationwidget.h
  selectforcefieldwidget.h
  selectspecieswidget.h
  wizardwidget.hui
  wizardwidgetfooter.h
  wizardwidgetheader.h
  wizardwidgethighlight.h
  wizardwidgetpageinfo.h

  # Dialogs
  addconfigurationdialog.h
  addforcefieldtermsdialog.h
  datamanagerdialog.h
  getconfigurationnamedialog.h
  getmodulelayernamedialog.h
  getspeciesnamedialog.h
  gettabnamedialog.h
  importspeciesdialog.h
  selectconfigurationdialog.h
  selectelementdialog.h
  selectforcefielddialog.h
  selectgenericitemdialog.h
  selectspeciesdialog.h
  selectsystemtemplatedialog.h
)
QT5_WRAP_CPP(subwindows_MOC_SRCS ${subwindows_MOC_HDRS})

# User Interface Files
SET(subwindows_UIS
  # Widgets
  addconfigurationwizard.ui
  addforcefieldtermswizard.ui
  importspecieswizard.ui
  selectconfigurationwidget.ui
  selectforcefieldwidget.ui
  selectspecieswidget.ui
  wizardwidgetfooter.ui
  wizardwidgetheader.ui

  # Dialogs
  addconfigurationdialog.ui
  addforcefieldtermsdialog.ui
  datamanagerdialog.ui
  getconfigurationnamedialog.ui
  getmodulelayernamedialog.ui
  getspeciesnamedialog.ui
  gettabnamedialog.ui
  importspeciesdialog.ui
  selectconfigurationdialog.ui
  selectelementdialog.ui
  selectforcefielddialog.ui
  selectgenericitemdialog.ui
  selectspeciesdialog.ui
  selectsystemtemplatedialog.ui
)
QT5_WRAP_UI(subwindows_UIS_H ${subwindows_UIS})

# Source files (not Qt UI files or custom .h [uih])
SET(subwindows_SRCS
  # Widgets
  addconfigurationwizard_funcs.cpp
  addforcefieldtermswizard_funcs.cpp
  importspecieswizard_funcs.cpp
  selectconfigurationwidget_funcs.cpp
  selectforcefieldwidget_funcs.cpp
  selectspecieswidget_funcs.cpp
  wizardwidget_funcs.cpp
  wizardwidgethighlight.cpp
  wizardwidgetpageinfo.cpp

  # Dialogs
  addconfigurationdialog_funcs.cpp
  addforcefieldtermsdialog_funcs.cpp
  datamanagerdialog_funcs.cpp
  getconfigurationnamedialog_funcs.cpp
  getmodulelayernamedialog_funcs.cpp
  getspeciesnamedialog_funcs.cpp
  gettabnamedialog_funcs.cpp
  importspeciesdialog_funcs.cpp
  selectconfigurationdialog_funcs.cpp
  selectelementdialog_funcs.cpp
  selectforcefielddialog_funcs.cpp
  selectgenericitemdialog_funcs.cpp
  selectspeciesdialog_funcs.cpp
  selectsystemtemplatedialog_funcs.cpp
)

# Target 'subwindows'
add_library(subwindows
  ${subwindows_UIS_H}
  ${subwindows_SRCS}
  ${subwindows_MOC_SRCS}
)
target_include_directories(subwindows PRIVATE
  ${PROJECT_SOURCE_DIR}/src
  ${PROJECT_BINARY_DIR}/src
  ${Qt5Widgets_INCLUDE_DIRS}
  ${FREETYPE_INCLUDE_DIRS}
)

target_link_libraries(subwindows widgets)

# -----------------
# Main Dissolve GUI
# -----------------

# Meta-Objects
SET(gui_MOC_HDRS
  # Editors
  modulecontrolwidget.h
  modulelisteditor.h
  modulewidget.h
  procedureeditor.h

  # Main Tabs
  configurationtab.h
  forcefieldtab.h
  layertab.h
  maintab.h
  moduletab.h
  speciestab.h
  workspacetab.h

  # Main UI
  gui.h
  stockcolours.h
  systemtemplate.h

  outputhandler.hui
  tmdiarea.hui
  thread.hui
  maintabsbar.hui
  maintabswidget.hui
)
QT5_WRAP_CPP(gui_MOC_SRCS ${gui_MOC_HDRS})

# User Interface Files
SET(gui_UIS
  # Editors
  modulecontrolwidget.ui
  modulelisteditor.ui
  procedureeditor.ui

  # Main Tabs
  configurationtab.ui
  forcefieldtab.ui
  layertab.ui
  moduletab.ui
  speciestab.ui
  workspacetab.ui

  # Main UI
  gui.ui
)
QT5_WRAP_UI(gui_UIS_H ${gui_UIS})

# Source files (not Qt UI files or custom .h [uih])
SET(gui_SRCS
  # Editors
  modulecontrolwidget_funcs.cpp
  modulelisteditor_funcs.cpp
  modulewidget.cpp
  procedureeditor_funcs.cpp

  # Main Tabs
  configurationtab_funcs.cpp
  forcefieldtab_funcs.cpp
  layertab_funcs.cpp
  maintab.cpp
  maintabsbar_funcs.cpp
  maintabswidget_funcs.cpp
  moduletab_funcs.cpp
  speciestab_funcs.cpp
  speciestab_geometry.cpp
  speciestab_isotopologues.cpp
  speciestab_sites.cpp
  workspacetab_funcs.cpp

  # Main UI
  gui_funcs.cpp
  gui_io.cpp
  gui_simulation.cpp
  gui_start.cpp
  menu_configuration.cpp
  menu_file.cpp
  menu_help.cpp
  menu_layer.cpp
  menu_simulation.cpp
  menu_species.cpp
  menu_workspace.cpp
  outputhandler_funcs.cpp
  referencepoint.cpp
  stockcolours.cpp
  systemtemplate.cpp
  thread_funcs.cpp
  tmdiarea_funcs.cpp

  referencepoint.h
)

# Resources
set(gui_RES_QRC
  main.qrc 
)
QT5_ADD_RESOURCES(gui_RES ${gui_RES_QRC})

# Target 'gui'
add_library(gui
  ${gui_RES}
  ${gui_UIS_H}
  ${gui_SRCS}
  ${gui_MOC_SRCS}
)
target_include_directories(gui PRIVATE
  ${PROJECT_SOURCE_DIR}/src
  ${PROJECT_BINARY_DIR}/src
  ${Qt5Widgets_INCLUDE_DIRS}
  ${FREETYPE_INCLUDE_DIRS}
)
target_link_libraries(gui keywordwidgets charts delegates viewer widgets gizmos)

# Subdirectories
add_subdirectory(keywordwidgets)
add_subdirectory(charts)
add_subdirectory(delegates)
add_subdirectory(gizmos)
add_subdirectory(viewer)
add_subdirectory(widgets)


# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!

# -----------------
# Main Dissolve GUI
# -----------------

# Meta-Objects
SET(gui_MOC_HDRS
  # Editors
  layereditor.h
  modulechartblock.h
  modulecharthotspot.h
  modulechartinsertionblock.h
  modulechartmetrics.h
  modulechartmoduleblock.h
  modulecontrolwidget.h
  modulelayertab.h
  moduletab.h
  modulewidget.h
  procedureeditor.h

  modulechart.hui

  # Widgets
  addconfigurationwizard.h
  addforcefieldtermswizard.h
  addprocessinglayerwizard.h
  addspecieswizard.h
  guidewizard.h
  keywordswidget.hui
  pairpotentialwidget.h
  wizardwidget.hui
  wizardwidgetfooter.h
  wizardwidgetheader.h
  wizardwidgethighlight.h
  wizardwidgetpageinfo.h

  # Dialogs
  addconfigurationdialog.h
  addforcefieldtermsdialog.h
  addprocessinglayerdialog.h
  addspeciesdialog.h
  getconfigurationnamedialog.h
  getspeciesnamedialog.h
  selectsystemtemplatedialog.h

  # Main Tabs
  configurationtab.h
  forcefieldtab.h
  maintab.h
  speciestab.h
  workspacetab.h

  # Main UI
  gui.h
  guide.h
  guidepage.h
  guidepagehighlight.h
  stockcolours.h
  systemtemplate.h

  outputhandler.hui
  tmdiarea.hui
  thread.hui
)
QT5_WRAP_CPP(gui_MOC_SRCS ${gui_MOC_HDRS})

# User Interface Files
SET(gui_UIS
  # Editors
  layereditor.ui
  modulechartinsertionblock.ui
  modulechartmoduleblock.ui
  modulecontrolwidget.ui
  modulelayertab.ui
  moduletab.ui
  procedureeditor.ui

  # Widgets
  addconfigurationwizard.ui
  addforcefieldtermswizard.ui
  addprocessinglayerwizard.ui
  addspecieswizard.ui
  guidewizard.ui
  pairpotentialwidget.ui
  wizardwidgetfooter.ui
  wizardwidgetheader.ui

  # Dialogs
  addconfigurationdialog.ui
  addforcefieldtermsdialog.ui
  addprocessinglayerdialog.ui
  addspeciesdialog.ui
  getconfigurationnamedialog.ui
  getspeciesnamedialog.ui
  selectsystemtemplatedialog.ui

  # Main Tabs
  configurationtab.ui
  forcefieldtab.ui
  speciestab.ui
  workspacetab.ui

  # Main UI
  gui.ui
)
QT5_WRAP_UI(gui_UIS_H ${gui_UIS})

# Source files (not Qt UI files or custom .h [uih])
SET(gui_SRCS
  # Editors
  layereditor_funcs.cpp
  modulechart_funcs.cpp
  modulechartblock.cpp
  modulecharthotspot.cpp
  modulechartinsertionblock_funcs.cpp
  modulechartmoduleblock_funcs.cpp
  modulecontrolwidget_funcs.cpp
  modulelayertab_funcs.cpp
  moduletab_funcs.cpp
  modulewidget.cpp
  procedureeditor_funcs.cpp

  # Widgets
  addconfigurationwizard_funcs.cpp
  addforcefieldtermswizard_funcs.cpp
  addprocessinglayerwizard_funcs.cpp
  addspecieswizard_funcs.cpp
  guidewizard_funcs.cpp
  keywordswidget_funcs.cpp
  pairpotentialwidget_funcs.cpp
  wizardwidget_funcs.cpp
  wizardwidgethighlight.cpp
  wizardwidgetpageinfo.cpp

  # Dialogs
  addconfigurationdialog_funcs.cpp
  addforcefieldtermsdialog_funcs.cpp
  addprocessinglayerdialog_funcs.cpp
  addspeciesdialog_funcs.cpp
  getconfigurationnamedialog_funcs.cpp
  getspeciesnamedialog_funcs.cpp
  selectsystemtemplatedialog_funcs.cpp

  # Main Tabs
  configurationtab_funcs.cpp
  forcefieldtab_funcs.cpp
  maintab.cpp
  speciestab_funcs.cpp
  workspacetab_funcs.cpp

  # Main UI
  gui_control.cpp
  gui_funcs.cpp
  gui_menu.cpp
  gui_io.cpp
  gui_start.cpp
  gui_tabs.cpp
  guide.cpp
  guidepage.cpp
  guidepagehighlight.cpp
  outputhandler_funcs.cpp
  stockcolours.cpp
  systemtemplate.cpp
  thread_funcs.cpp
  tmdiarea_funcs.cpp
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

add_subdirectory(charts)
add_subdirectory(delegates)
add_subdirectory(keywordwidgets)
add_subdirectory(viewer)
add_subdirectory(widgets)

target_link_libraries(gui keywordwidgets charts delegates viewer widgets)


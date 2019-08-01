# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!

# -----------------
# Main Dissolve GUI
# -----------------

# Meta-Objects
SET(gui_MOC_HDRS
  # Layer Editor
  layereditor.h
  modulechartblock.h
  modulecharthotspot.h
  modulechartinsertionblock.h
  modulechartmetrics.h
  modulechartmoduleblock.h
  modulekeywordswidget.h
  modulecontrolwidget.h
  modulelayertab.h
  moduletab.h
  modulewidget.h

  modulechart.hui

  # Widgets
  addconfigurationwizard.h
  addforcefieldtermswizard.h
  addprocessinglayerwizard.h
  addspecieswizard.h
  guidewizard.h
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

  outputhandler.hui
  tmdiarea.hui
  thread.hui
)
QT5_WRAP_CPP(gui_MOC_SRCS ${gui_MOC_HDRS})

# User Interface Files
SET(gui_UIS
  # Layer Editor
  layereditor.ui
  modulechartinsertionblock.ui
  modulechartmoduleblock.ui
  modulecontrolwidget.ui
  modulelayertab.ui
  moduletab.ui

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
  # Layer Editor
  layereditor_funcs.cpp
  modulechart_funcs.cpp
  modulechartblock.cpp
  modulecharthotspot.cpp
  modulechartinsertionblock_funcs.cpp
  modulechartmoduleblock_funcs.cpp
  modulecontrolwidget_funcs.cpp
  modulekeywordswidget_funcs.cpp
  modulelayertab_funcs.cpp
  moduletab_funcs.cpp
  modulewidget.cpp

  # Widgets
  addconfigurationwizard_funcs.cpp
  addforcefieldtermswizard_funcs.cpp
  addprocessinglayerwizard_funcs.cpp
  addspecieswizard_funcs.cpp
  guidewizard_funcs.cpp
  pairpotentialwidget_funcs.cpp
  wizardwidget_funcs.cpp
  wizardwidgethighlight.cpp
  wizardwidgetpageinfo.cpp

  # Dialogs
  addconfigurationdialog_funcs.cpp
  addforcefieldtermsdialog_funcs.cpp
  addprocessinglayerdialog_funcs.cpp
  addspeciesdialog_funcs.cpp

  # Main Tabs
  configurationtab_funcs.cpp
  forcefieldtab_funcs.cpp
  maintab.cpp
  speciestab_funcs.cpp
  workspacetab_funcs.cpp

  # Main UI
  gui_funcs.cpp
  gui_menu.cpp
  gui_simulation_control.cpp
  gui_simulation_state.cpp
  gui_simulation_tabs.cpp
  gui_start.cpp
  guide.cpp
  guidepage.cpp
  guidepagehighlight.cpp
  outputhandler_funcs.cpp
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

# ---------------------
# Dissolve Guide Editor
# ---------------------

# Meta-Objects
SET(ed_MOC_HDRS
  dguided.h
)
QT5_WRAP_CPP(ed_MOC_SRCS ${ed_MOC_HDRS})

# User Interface Files
SET(ed_UIS
  dguided.ui
)
QT5_WRAP_UI(ed_UIS_H ${ed_UIS})

# Source files (not Qt UI files or custom .h [uih])
SET(ed_SRCS
  dguided_funcs.cpp
)

# Target 'ed'
add_library(ed
  ${ed_UIS_H}
  ${ed_SRCS}
  ${ed_MOC_SRCS}
)
target_include_directories(ed PRIVATE
  ${PROJECT_SOURCE_DIR}/src
  ${PROJECT_BINARY_DIR}/src
  ${Qt5Widgets_INCLUDE_DIRS}
)

add_subdirectory(delegates)
add_subdirectory(keywordwidgets)
add_subdirectory(viewer)
add_subdirectory(widgets)

target_link_libraries(gui keywordwidgets delegates viewer widgets)


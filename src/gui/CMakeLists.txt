# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!

# Meta-Objects
SET(gui_MOC_HDRS
  # Module Chart
  modulechartblock.h
  modulecharthotspot.h
  modulechartinsertionblock.h
  modulechartmetrics.h
  modulechartmoduleblock.h
  modulekeywordswidget.h
  modulecontrolwidget.h
  moduleeditor.h
  moduletab.h
  modulewidget.h
  paletteblock.h

  modulechart.hui
  modulepalette.hui
  # Widgets
  addspecieswizard.h
  pairpotentialwidget.h
  wizardwidget.hui
  wizardwidgetfooter.h
  wizardwidgetheader.h
  wizardwidgethighlight.h
  wizardwidgetpageinfo.h
  # Dialogs
  addspeciesdialog.h
  # Main Tabs
  analysistab.h
  configurationtab.h
  forcefieldtab.h
  maintab.h
  processingtab.h
  speciestab.h
  workspacetab.h
  # Main UI
  gui.h

  outputhandler.hui
  tmdiarea.hui
  thread.hui
)
QT5_WRAP_CPP(gui_MOC_SRCS ${gui_MOC_HDRS})

# User Interface Files
SET(gui_UIS
  # Module Chart
  modulechartinsertionblock.ui
  modulechartmoduleblock.ui
  modulecontrolwidget.ui
  moduleeditor.ui
  moduletab.ui
  paletteblock.ui
  # Widgets
  addspecieswizard.ui
  pairpotentialwidget.ui
  wizardwidgetfooter.ui
  wizardwidgetheader.ui
  # Dialogs
  addspeciesdialog.ui
  # Main Tabs
  analysistab.ui
  configurationtab.ui
  forcefieldtab.ui
  processingtab.ui
  speciestab.ui
  workspacetab.ui
  # Main UI
  gui.ui
)
QT5_WRAP_UI(gui_UIS_H ${gui_UIS})

# Source files (not Qt UI files or custom .h [uih])
SET(gui_SRCS
  # Module Chart
  modulechart_funcs.cpp
  modulechartblock.cpp
  modulecharthotspot.cpp
  modulechartinsertionblock_funcs.cpp
  modulechartmoduleblock_funcs.cpp
  modulecontrolwidget_funcs.cpp
  moduleeditor_funcs.cpp
  modulekeywordswidget_funcs.cpp
  modulepalette_funcs.cpp
  moduletab_funcs.cpp
  modulewidget.cpp
  paletteblock_funcs.cpp
  # Widgets
  addspecieswizard_funcs.cpp
  pairpotentialwidget_funcs.cpp
  wizardwidget_funcs.cpp
  wizardwidgethighlight.cpp
  wizardwidgetpageinfo.cpp
  # Dialogs
  addspeciesdialog_funcs.cpp
  # Main Tabs
  analysistab_funcs.cpp
  configurationtab_funcs.cpp
  forcefieldtab_funcs.cpp
  maintab.cpp
  processingtab_funcs.cpp
  speciestab_funcs.cpp
  workspacetab_funcs.cpp
  # Main UI
  gui_funcs.cpp
  gui_menu.cpp
  gui_simulation_control.cpp
  gui_simulation_state.cpp
  gui_simulation_tabs.cpp
  gui_stack.cpp
  gui_start.cpp
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

add_subdirectory(delegates)
add_subdirectory(keywordwidgets)
add_subdirectory(uchroma)
add_subdirectory(widgets)

target_link_libraries(gui keywordwidgets delegates widgets uchroma)

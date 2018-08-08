# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!

# Meta-Objects
SET(gui_MOC_HDRS
  analysistab.h
  configurationtab.h
  forcefieldtab.h
  gui.h
  maintab.h
  modulechartblock.h
  modulecharthotspot.h
  modulechartinsertionblock.h
  modulechartmoduleblock.h
  modulekeywordswidget.h
  modulecontrolwidget.h
  moduleeditor.h
  moduletab.h
  modulewidget.h
  pairpotentialwidget.h
  paletteblock.h
  processingtab.h
  speciestab.h
  workspacetab.h

  modulechart.hui
  modulepalette.hui
  outputhandler.hui
  tmdiarea.hui
  thread.hui
)
QT5_WRAP_CPP(gui_MOC_SRCS ${gui_MOC_HDRS})

# User Interface Files
SET(gui_UIS
  analysistab.ui
  configurationtab.ui
  forcefieldtab.ui
  gui.ui
  modulechartinsertionblock.ui
  modulechartmoduleblock.ui
  modulecontrolwidget.ui
  moduleeditor.ui
  moduletab.ui
  pairpotentialwidget.ui
  paletteblock.ui
  processingtab.ui
  speciestab.ui
  workspacetab.ui
)
QT5_WRAP_UI(gui_UIS_H ${gui_UIS})

# Source files (not Qt UI files or custom .h [uih])
SET(gui_SRCS
  analysistab_funcs.cpp
  configurationtab_funcs.cpp
  forcefieldtab_funcs.cpp
  gui_control.cpp
  gui_funcs.cpp
  gui_menu.cpp
  gui_tabs.cpp
  maintab.cpp
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
  outputhandler_funcs.cpp
  pairpotentialwidget_funcs.cpp
  paletteblock_funcs.cpp
  processingtab_funcs.cpp
  speciestab_funcs.cpp
  thread_funcs.cpp
  tmdiarea_funcs.cpp
  workspacetab_funcs.cpp
)

# Resources
set(gui_RES_QRC
  icons.qrc 
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

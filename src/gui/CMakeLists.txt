# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!

# Meta-Objects
SET(gui_MOC_HDRS
  browser.h
  gui.h
  maintab.h
  mimestrings.h
  mimetreewidgetitem.h
  mastertermswidget.h
  modulecontrolwidget.h
  modulewidget.h
  pairpotentialwidget.h
  subwidget.h
  subwindow.h
  tmenubuttongroup.h
  tablewidgetupdater.h
  workspacetab.h

  colourbutton.hui
  gradientbar.hui
  mimetreewidget.hui
  outputhandler.hui
  texponentialspin.hui
  texponentialspindelegate.hui
  thread.hui
  tmenubutton.hui
  tpopupwidget.hui
)
QT5_WRAP_CPP(gui_MOC_SRCS ${gui_MOC_HDRS})

# User Interface Files
SET(gui_UIS
  browser.ui
  gui.ui
  mastertermswidget.ui
  modulecontrolwidget.ui
  pairpotentialwidget.ui
  workspacetab.ui
)
QT5_WRAP_UI(gui_UIS_H ${gui_UIS})

# Source files (not Qt UI files or custom .h [uih])
SET(gui_SRCS
  browser_funcs.cpp
  colourbutton_funcs.cpp
  gui_funcs.cpp
  gradientbar_funcs.cpp
  maintab.cpp
  mimestrings.cpp
  mimetreewidget_funcs.cpp
  mimetreewidgetitem.cpp
  mastertermswidget_funcs.cpp
  modulecontrolwidget_funcs.cpp
  modulewidget.cpp
  outputhandler_funcs.cpp
  pairpotentialwidget_funcs.cpp
  subwidget.cpp
  subwindow.cpp
  texponentialspin_funcs.cpp
  texponentialspindelgate_funcs.cpp
  thread_funcs.cpp
  tmenubutton_funcs.cpp
  tmenubuttongroup.cpp
  tpopupwidget_funcs.cpp
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

add_subdirectory(keywordwidgets)
add_subdirectory(uchroma)

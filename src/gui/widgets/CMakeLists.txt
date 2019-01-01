# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!

# Meta-Objects
SET(widgets_MOC_HDRS
  mimestrings.h
  mimetreewidgetitem.h
  nocontrols.h
  subwidget.h
  subwindow.h
  tmenubuttongroup.h

  checkindicator.hui
  colourbutton.hui
  gradientbar.hui
  mimetreewidget.hui
  supertabbar.hui
  supertabwidget.hui
  texponentialspin.hui
  tmenubutton.hui
  tpopupwidget.hui
)
QT5_WRAP_CPP(widgets_MOC_SRCS ${widgets_MOC_HDRS})

# User Interface Files
SET(widgets_UIS
  nocontrols.ui
)
QT5_WRAP_UI(widgets_UIS_H ${widgets_UIS})

# Source files (not Qt UI files or custom .h [uih])
SET(widgets_SRCS
  checkindicator_funcs.cpp
  colourbutton_funcs.cpp
  gradientbar_funcs.cpp
  mimestrings.cpp
  mimetreewidget_funcs.cpp
  mimetreewidgetitem.cpp
  nocontrols_funcs.cpp
  subwidget.cpp
  subwindow.cpp
  supertabbar_funcs.cpp
  supertabwidget_funcs.cpp
  texponentialspin_funcs.cpp
  tmenubutton_funcs.cpp
  tmenubuttongroup.cpp
  tpopupwidget_funcs.cpp
)

# Target 'gui'
add_library(widgets
  ${widgets_UIS_H}
  ${widgets_SRCS}
  ${widgets_MOC_SRCS}
)
target_include_directories(widgets PRIVATE
  ${PROJECT_SOURCE_DIR}/src
  ${PROJECT_BINARY_DIR}/src
  ${Qt5Widgets_INCLUDE_DIRS}
)


# GUI Module Library

set(lib_name testmodulegui)

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!

# Meta-Objects
SET(${lib_name}_MOC_HDRS
  modulewidget.h
)
QT5_WRAP_CPP(${lib_name}_MOC_SRCS ${${lib_name}_MOC_HDRS})


# User Interface Files
SET(${lib_name}_UIS
  modulewidget.ui
)
QT5_WRAP_UI(${lib_name}_UIS_H ${${lib_name}_UIS})

# Source files (not Qt UI files or custom .h [uih])
SET(${lib_name}_SRCS
  gui.cpp
  modulewidget_funcs.cpp
)

add_library(${lib_name}
  ${${lib_name}_UIS_H}
  ${${lib_name}_SRCS}
  ${${lib_name}_MOC_SRCS}
)

target_include_directories(${lib_name} PRIVATE
  ${PROJECT_SOURCE_DIR}/src
  ${PROJECT_BINARY_DIR}/src
  ${Qt5Core_INCLUDE_DIRS}
  ${Qt5Gui_INCLUDE_DIRS}
  ${Qt5Widgets_INCLUDE_DIRS}
  ${FREETYPE_INCLUDE_DIRS}
)


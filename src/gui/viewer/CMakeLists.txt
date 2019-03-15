# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!

# Meta-Objects
SET(viewer_MOC_HDRS
  datawidget.h
  selectsymbol.h

  #configurationviewer.hui
  dataviewer.hui
  speciesviewer.hui
  viewer.hui
)
QT5_WRAP_CPP(viewer_MOC_SRCS ${viewer_MOC_HDRS})

# User Interface Files
SET(viewer_UIS
  datawidget.ui
  specieswidget.ui
}
QT5_WRAP_UI(viewer_UIS_H ${viewer_UIS})

# Source files (not Qt UI files or custom .h [uih])
SET(viewer_SRCS
  #configurationviewer_input.cpp
  #configurationviewer_scene.cpp
  dataviewer_contextmenu.cpp
  dataviewer_funcs.cpp
  dataviewer_input.cpp
  dataviewer_interaction.cpp
  dataviewer_query.cpp
  dataviewer_render.cpp
  datawidget_funcs.cpp
  selectsymbol_funcs.cpp
  speciesviewer_funcs.cpp
  speciesviewer_input.cpp
  speciesviewer_scene.cpp
  viewer_data.cpp
  viewer_funcs.cpp
  viewer_input.cpp
  viewer_io.cpp
  viewer_render.cpp
)

# User Interface Files
SET(viewer_UIS
  datawidget.ui
  selectsymbol.ui
}
QT5_WRAP_UI(viewer_UIS_H ${viewer_UIS})

# Target 'viewer'
add_library(viewer
  ${viewer_UIS_H}
  ${viewer_SRCS}
  ${viewer_MOC_SRCS}
)
target_include_directories(viewer PRIVATE
  ${PROJECT_SOURCE_DIR}/src
  ${PROJECT_BINARY_DIR}/src
  ${Qt5Widgets_INCLUDE_DIRS}
)


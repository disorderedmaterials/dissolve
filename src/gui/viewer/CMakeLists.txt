# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!

# Meta-Objects
SET(viewer_MOC_HDRS
  # configurationviewer.hui
  dataviewer.hui
  speciesviewer.hui
  viewer.hui
)
QT5_WRAP_CPP(viewer_MOC_SRCS ${viewer_MOC_HDRS})

# Source files (not Qt UI files or custom .h [uih])
SET(viewer_SRCS
  graphwidget.h
  selectsymbol.h

  configurationviewer_input.cpp
  configurationviewer_scene.cpp
  dataviewer_contextmenu.cpp
  dataviewer_funcs.cpp
  dataviewer_input.cpp
  dataviewer_interaction.cpp
  dataviewer_io.cpp
  dataviewer_render.cpp
  graphwidget_funcs.cpp
  selectsymbol_funcs.cpp
  speciesviewer_contextmenu.cpp
  speciesviewer_funcs.cpp
  speciesviewer_input.cpp
  speciesviewer_interaction.cpp
  speciesviewer_render.cpp
  viewer_data.cpp
  viewer_funcs.cpp
  viewer_input.cpp
  viewer_interaction.cpp
  viewer_query.cpp
  viewer_render.cpp
)

# Target 'viewer'
add_library(viewer
  ${viewer_SRCS}
  ${viewer_MOC_SRCS}
)
target_include_directories(viewer PRIVATE
  ${PROJECT_SOURCE_DIR}/src
  ${PROJECT_BINARY_DIR}/src
  ${Qt5Widgets_INCLUDE_DIRS}
)


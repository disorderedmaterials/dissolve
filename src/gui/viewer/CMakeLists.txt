# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!

# Meta-Objects
SET(viewer_MOC_HDRS
  datawidget.h
  selectsymbol.h
  specieswidget.h

  configurationviewer.hui
  dataviewer.hui
  speciesviewer.hui
  viewer.hui
)
QT5_WRAP_CPP(viewer_MOC_SRCS ${viewer_MOC_HDRS})

# User Interface Files
SET(viewer_UIS
  configurationwidget.ui
  datawidget.ui
  selectsymbol.ui
  specieswidget.ui
)
QT5_WRAP_UI(viewer_UIS_H ${viewer_UIS})

# Source files (not Qt UI files or custom .h [uih])
SET(viewer_SRCS
  configurationviewer_funcs.cpp
  configurationviewer_input.cpp
  configurationviewer_interaction.cpp
  configurationviewer_render.cpp
  configurationviewer_scene.cpp
  configurationwidget_funcs.cpp
  dataviewer_funcs.cpp
  dataviewer_input.cpp
  dataviewer_interaction.cpp
  dataviewer_render.cpp
  datawidget_funcs.cpp
  selectsymbol_funcs.cpp
  speciesviewer_funcs.cpp
  speciesviewer_input.cpp
  speciesviewer_interaction.cpp
  speciesviewer_render.cpp
  specieswidget_funcs.cpp
  viewer_data.cpp
  viewer_funcs.cpp
  viewer_input.cpp
  viewer_interaction.cpp
  viewer_io.cpp
  viewer_query.cpp
  viewer_render.cpp
)

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
  ${FREETYPE_INCLUDE_DIRS}
)

add_subdirectory(render)

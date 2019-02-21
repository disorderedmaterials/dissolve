# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!

# Meta-Objects
SET(viewer_MOC_HDRS
  # configurationviewer.hui
  speciesviewer.hui
  viewer.hui
)
QT5_WRAP_CPP(viewer_MOC_SRCS ${viewer_MOC_HDRS})

# Source files (not Qt UI files or custom .h [uih])
SET(viewer_SRCS
#configurationviewer_input.cpp
#configurationviewer_scene.cpp
  speciesviewer_funcs.cpp
  speciesviewer_input.cpp
  speciesviewer_render.cpp
  primitive.cpprprimitiveinfo.cpp
  primitivevertex.cpp
  textprimitive.cpp
  vertexchunk.cpp
  viewer_engine.cpp
  viewer_funcs.cpp
  viewer_input.cpp
  viewer_prefs.cpp
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


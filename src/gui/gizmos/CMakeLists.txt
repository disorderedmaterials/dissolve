# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!

# Meta-Objects
SET(gizmos_MOC_HDRS
  integrator1d.h
)
QT5_WRAP_CPP(gizmos_MOC_SRCS ${gizmos_MOC_HDRS})

# User Interface Files
SET(gizmos_UIS
  integrator1d.ui
)
QT5_WRAP_UI(gizmos_UIS_H ${gizmos_UIS})

# Source files (not Qt UI files or custom .h [uih])
SET(gizmos_SRCS
  integrator1d_funcs.cpp
)

# Target 'gui'
add_library(gizmos
  ${gizmos_UIS_H}
  ${gizmos_SRCS}
  ${gizmos_MOC_SRCS}
)
target_include_directories(gizmos PRIVATE
  ${PROJECT_SOURCE_DIR}/src
  ${PROJECT_BINARY_DIR}/src
  ${Qt5Widgets_INCLUDE_DIRS}
)


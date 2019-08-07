# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!

# Meta-Objects
SET(editors_MOC_HDRS
  procedure.h
)
QT5_WRAP_CPP(editors_MOC_SRCS ${editors_MOC_HDRS})

# User Interface Files
SET(editors_UIS
  procedure.ui
)
QT5_WRAP_UI(editors_UIS_H ${editors_UIS})

# Source files (not Qt UI files or custom .h [uih])
SET(editors_SRCS
  procedures_funcs.cpp
)

# Target 'editors'
add_library(editors
  ${editors_UIS_H}
  ${editors_SRCS}
  ${editors_MOC_SRCS}
)
target_include_directories(editors PRIVATE
  ${PROJECT_SOURCE_DIR}/src
  ${PROJECT_BINARY_DIR}/src
  ${Qt5Widgets_INCLUDE_DIRS}
)


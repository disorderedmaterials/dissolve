# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!

# Meta-Objects
SET(charts_MOC_HDRS
  procedure.hui
)
QT5_WRAP_CPP(charts_MOC_SRCS ${charts_MOC_HDRS})

# User Interface Files
SET(charts_UIS
  procedurenode.ui
)
QT5_WRAP_UI(charts_UIS_H ${charts_UIS})

# Source files (not Qt UI files or custom .h [uih])
SET(charts_SRCS
  chartblock.cpp
  charthotspot.cpp
  procedure_funcs.cpp
  procedurenode_funcs.cpp

  chartblock.h
  charthotspot.h
  proceduremetrics.h
  procedurenode.h
)

# Target 'charts'
add_library(charts
  ${charts_UIS_H}
  ${charts_SRCS}
  ${charts_MOC_SRCS}
)
target_include_directories(charts PRIVATE
  ${PROJECT_SOURCE_DIR}/src
  ${PROJECT_BINARY_DIR}/src
  ${Qt5Widgets_INCLUDE_DIRS}
)


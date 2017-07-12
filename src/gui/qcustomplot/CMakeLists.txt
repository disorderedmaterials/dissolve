# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
# Meta-Objects
SET(qcustomplot_MOC_HDRS
  qcustomplot.hui
  tplotwidget.hui
)
QT5_WRAP_CPP(qcustomplot_MOC_SRCS ${qcustomplot_MOC_HDRS})

# Source files
set(qcustomplot_SRCS
  plotdata.cpp
  plotdatastyle.cpp
  qcustomplot_funcs.cpp
  tplotwidget_funcs.cpp
)

# Target 'qcustomplot'
add_library(qcustomplot STATIC
  ${qcustomplot_SRCS}
  ${qcustomplot_MOC_SRCS}
)
target_include_directories(qcustomplot PRIVATE
  ${PROJECT_SOURCE_DIR}/src
  ${PROJECT_BINARY_DIR}/src
  ${Qt5Widgets_INCLUDE_DIRS}
  ${FREETYPE_INCLUDE_DIRS}
)

target_link_libraries(qcustomplot)

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!

# Meta-Objects
SET(delegates_MOC_HDRS
  combolist.hui
  integerspin.hui
  intraformcombo.hui
  isotopecombo.hui
  texponentialspin.hui
)
QT5_WRAP_CPP(delegates_MOC_SRCS ${delegates_MOC_HDRS})

# User Interface Files
#SET(delegates_UIS
#)
#QT5_WRAP_UI(delegates_UIS_H ${delegates_UIS})

# Source files (not Qt UI files or custom .h [uih])
SET(delegates_SRCS
  combolist_funcs.cpp
  integerspin_funcs.cpp
  intraformcombo_funcs.cpp
  isotopecombo_funcs.cpp
  texponentialspin_funcs.cpp
)

# Target 'delegates'
add_library(delegates
# ${delegates_UIS_H}
  ${delegates_SRCS}
  ${delegates_MOC_SRCS}
)
target_include_directories(delegates PRIVATE
  ${PROJECT_SOURCE_DIR}/src
  ${PROJECT_BINARY_DIR}/src
  ${Qt5Widgets_INCLUDE_DIRS}
)


# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!

# Meta-Objects
SET(keywordwidgets_MOC_HDRS
  bool.hui
  charstring.hui
  double.hui
  int.hui
  windowfunction.hui
)
QT5_WRAP_CPP(keywordwidgets_MOC_SRCS ${keywordwidgets_MOC_HDRS})

# Source files (not Qt UI files or custom .h [uih])
SET(keywordwidgets_SRCS
  base.cpp

  bool_funcs.cpp
  charstring_funcs.cpp
  double_funcs.cpp
  int_funcs.cpp
  windowfunction_funcs.cpp

  base.h
)

# Target 'keywordwidgets'
add_library(keywordwidgets
  ${keywordwidgets_SRCS}
  ${keywordwidgets_MOC_SRCS}
)
target_include_directories(keywordwidgets PRIVATE
  ${PROJECT_SOURCE_DIR}/src
  ${PROJECT_BINARY_DIR}/src
  ${Qt5Widgets_INCLUDE_DIRS}
)

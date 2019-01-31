# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!

# Meta-Objects
SET(keywordwidgets_MOC_HDRS
  atomtypeselection.h
  broadeningfunction.h
  isotopologuelist.h
  pairbroadeningfunction.h
  dropdown.h

  bool.hui
  charstring.hui
  double.hui
  dropwidget.hui
  enumstring.hui
  int.hui
  windowfunction.hui
)
QT5_WRAP_CPP(keywordwidgets_MOC_SRCS ${keywordwidgets_MOC_HDRS})

# User Interface Files
SET(keywordwidgets_UIS
  atomtypeselection.ui
  broadeningfunction.ui
  dropdown.ui
  isotopologuelist.ui
  pairbroadeningfunction.ui
)
QT5_WRAP_UI(keywordwidgets_UIS_H ${keywordwidgets_UIS})

# Source files (not Qt UI files or custom .h [uih])
SET(keywordwidgets_SRCS
  base.cpp

  atomtypeselection_funcs.cpp
  bool_funcs.cpp
  broadeningfunction_funcs.cpp
  charstring_funcs.cpp
  double_funcs.cpp
  dropdown_funcs.cpp
  dropwidget_funcs.cpp
  enumstring_funcs.cpp
  int_funcs.cpp
  isotopologuelist_funcs.cpp
  pairbroadeningfunction_funcs.cpp
  windowfunction_funcs.cpp

  base.h
)

# Target 'keywordwidgets'
add_library(keywordwidgets
  ${keywordwidgets_UIS_H}
  ${keywordwidgets_SRCS}
  ${keywordwidgets_MOC_SRCS}
)
target_include_directories(keywordwidgets PRIVATE
  ${PROJECT_SOURCE_DIR}/src
  ${PROJECT_BINARY_DIR}/src
  ${Qt5Widgets_INCLUDE_DIRS}
)

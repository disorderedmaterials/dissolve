add_library(uchromakernels
  fit.cpp
  fit_sd.cpp
  fit_sdmod.cpp
  fit_simplex.cpp
  fit.h
)

target_include_directories(uchromakernels PRIVATE
  ${PROJECT_SOURCE_DIR}/src
  ${PROJECT_BINARY_DIR}/src
  ${Qt5Core_INCLUDE_DIRS}
  ${Qt5Widgets_INCLUDE_DIRS}
  ${Qt5Gui_INCLUDE_DIRS}
  ${FREETYPE_INCLUDE_DIRS}
)

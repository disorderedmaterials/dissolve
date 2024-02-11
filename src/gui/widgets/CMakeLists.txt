# Target 'widgets'
add_library(
  widgets
  checkIndicator.cpp
  checkIndicator.h
  colourButton.cpp
  colourButton.h
  elementSelector.cpp
  elementSelector.h
  exponentialSpin.cpp
  exponentialSpin.h
  gradientBar.cpp
  gradientBar.h
  integerSpin.cpp
  integerSpin.h
  noControls.cpp
  noControls.h
  noControls.ui
  sampledDoubleLabel.cpp
  sampledDoubleLabel.h
  sampledDoubleLabel.ui
  superStackedWidget.cpp
  superStackedWidget.h
)
target_include_directories(widgets PRIVATE ${PROJECT_SOURCE_DIR}/src ${PROJECT_BINARY_DIR}/src ${Qt6Widgets_INCLUDE_DIRS})
target_link_libraries(widgets PRIVATE base)

# Because Qt has its own way of checking for C++17
if(MSVC)
  target_compile_options(widgets PUBLIC /Zc:__cplusplus)
endif(MSVC)

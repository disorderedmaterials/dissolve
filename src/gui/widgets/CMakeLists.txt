# Don't forget to include output directory, otherwise the UI file won't be wrapped!

# Meta-Objects
set(widgets_MOC_HDRS
    noControls.h
    sampledDoubleLabel.h
    checkIndicator.hui
    colourButton.hui
    elementSelector.hui
    exponentialSpin.hui
    gradientBar.hui
    integerSpin.hui
    superStackedWidget.hui
)
qt6_wrap_cpp(widgets_MOC_SRCS ${widgets_MOC_HDRS})

# User Interface Files
set(widgets_UIS noControls.ui sampledDoubleLabel.ui)
qt6_wrap_ui(widgets_UIS_H ${widgets_UIS})

# Source files (not Qt UI files or custom .h [uih])
set(widgets_SRCS
    checkindicatorFuncs.cpp
    colourbuttonFuncs.cpp
    elementselectorFuncs.cpp
    exponentialspinFuncs.cpp
    gradientbarFuncs.cpp
    integerspinFuncs.cpp
    nocontrolsFuncs.cpp
    sampleddoublelabelFuncs.cpp
    superstackedwidgetFuncs.cpp
)

# Target 'gui'
add_library(widgets ${widgets_UIS_H} ${widgets_SRCS} ${widgets_MOC_SRCS})
target_include_directories(widgets PRIVATE ${PROJECT_SOURCE_DIR}/src ${PROJECT_BINARY_DIR}/src ${Qt6Widgets_INCLUDE_DIRS})
target_link_libraries(widgets PRIVATE base)

# Because Qt has its own way of checking for C++17
if(MSVC)
  target_compile_options(widgets PUBLIC /Zc:__cplusplus)
endif(MSVC)

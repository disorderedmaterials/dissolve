# Target 'delegates'
add_library(
  delegates
  comboList.cpp
  comboList.h
  exponentialSpin.cpp
  exponentialSpin.h
  integerSpin.cpp
  integerSpin.h
  isotopeCombo.cpp
  isotopeCombo.h
  null.cpp
  customComboDelegate.h
  intraFormCombo.h
  null.h
)
target_include_directories(delegates PRIVATE ${PROJECT_SOURCE_DIR}/src ${PROJECT_BINARY_DIR}/src ${Qt6Widgets_INCLUDE_DIRS})
target_link_libraries(delegates PRIVATE base)

# Because Qt has its own way of checking for C++17
if(MSVC)
  target_compile_options(delegates PUBLIC /Zc:__cplusplus)
endif(MSVC)

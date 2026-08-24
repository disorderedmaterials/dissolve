# Target 'gui'
add_library(
  gui
  # Qt Resources
  main.qrc
  # Core Classes
  wizard.cpp
  wizard.h
  wizardFooter.h
  wizardFooter.ui
  wizardHeader.h
  wizardHeader.ui
)

target_include_directories(
  gui PRIVATE ${PROJECT_SOURCE_DIR}/src ${PROJECT_BINARY_DIR}/src ${Qt6Core_INCLUDE_DIRS} ${Qt6Widgets_INCLUDE_DIRS}
)

target_link_libraries(
  gui
  PRIVATE delegates
          widgets
          models
          base
          Qt6::Quick
          Qt6::QuickControls2
          Qt6::Core
          Qt6::Gui
          Qt6::Qml
          Qt6::QmlModels
          Qt6::QuickWidgets
)

# Subdirectories
add_subdirectory(delegates)
add_subdirectory(widgets)
add_subdirectory(models)
add_subdirectory(icons)

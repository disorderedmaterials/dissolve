# Target 'gui'
add_library(
  gui-qml
  # Qt Resources
  resources.qrc
)

target_include_directories(
  gui-qml
  PRIVATE ${PROJECT_SOURCE_DIR}/src
          ${PROJECT_BINARY_DIR}/src
          ${Qt6Widgets_INCLUDE_DIRS}
          ${FREETYPE_INCLUDE_DIRS}
          Qt6::OpenGL
          Qt6::OpenGLWidgets
          Qt6::Core
)

target_link_libraries(
  gui-qml
  PRIVATE models
          base
          Qt6::OpenGL
          Qt6::OpenGLWidgets
          Qt6::Quick
          Qt6::QuickControls2
          Qt6::Core
          Qt6::Gui
          Qt6::Qml
          Qt6::QmlModels
          Qt6::QuickWidgets
)

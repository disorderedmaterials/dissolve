add_library(gui)

target_include_directories(
    qml-gui
    PRIVATE ${PROJECT_SOURCE_DIR}/src
            ${PROJECT_BINARY_DIR}/src
            ${Qt6Widgets_INCLUDE_DIRS}
            ${FREETYPE_INCLUDE_DIRS}
            Qt6::OpenGL
            Qt6::OpenGLWidgets
            Qt6::Core
  )

  target_link_libraries(
    qml-gui
    PRIVATE models
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

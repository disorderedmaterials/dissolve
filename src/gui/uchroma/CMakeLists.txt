add_subdirectory(classes)
add_subdirectory(expression)
add_subdirectory(gui)
add_subdirectory(kernels)
add_subdirectory(render)
add_subdirectory(session)

add_library(uchroma
  uchromabase_collections.cpp
  uchromabase_funcs.cpp
  uchromabase_image.cpp
  uchromabase_interaction.cpp
  uchromabase_signal.cpp
  uchromabase_undoredo.cpp
  uchromabase_view.cpp

  uchromabase.h
)

target_include_directories(uchroma PRIVATE
  ${PROJECT_SOURCE_DIR}/src
  ${PROJECT_BINARY_DIR}/src
  ${Qt5Core_INCLUDE_DIRS}
  ${Qt5Widgets_INCLUDE_DIRS}
  ${Qt5Gui_INCLUDE_DIRS}
  ${FREETYPE_INCLUDE_DIRS}
)


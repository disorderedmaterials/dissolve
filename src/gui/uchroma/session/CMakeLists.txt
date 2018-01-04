add_library(uchromasession
  editstate.cpp
  editstate_axes.cpp
  editstate_collection.cpp
  editstate_viewpane.cpp
  editstatedata.cpp
  editstategroup.cpp
  keywords.cpp
  load.cpp
  save.cpp
  editstate.h
  editstatedata.h
  editstategroup.h
)

target_include_directories(uchromasession PRIVATE
  ${PROJECT_SOURCE_DIR}/src
  ${PROJECT_BINARY_DIR}/src
  ${Qt5Core_INCLUDE_DIRS}
  ${Qt5Widgets_INCLUDE_DIRS}
  ${Qt5Gui_INCLUDE_DIRS}
  ${FREETYPE_INCLUDE_DIRS}
)

target_link_libraries(uchromasession uchromagui)

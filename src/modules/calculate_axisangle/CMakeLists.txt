# Main Module Library
add_library(calculateaxisanglemodule
  core.cpp
  definition.cpp
  functions.cpp
  init.cpp
  process.cpp

  axisangle.h
)

# 'nogui' library
add_library(calculateaxisanglemodulenogui
  nogui.cpp
)

# gui library
if(GUI)
  add_subdirectory(gui)
endif(GUI)

include_directories(
  ${PROJECT_SOURCE_DIR}/src
  ${PROJECT_BINARY_DIR}/src
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_BINARY_DIR}
  ${CMAKE_BINARY_DIR}/src
)

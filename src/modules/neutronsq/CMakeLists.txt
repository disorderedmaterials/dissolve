# Main Module Library
add_library(neutronsqmodule
  core.cpp
  definition.cpp
  functions.cpp
  init.cpp
  process.cpp

  neutronsq.h
)

# 'nogui' library
add_library(neutronsqmodulenogui
  nogui.cpp
)

# gui library
if(GUI)
  add_subdirectory(gui)
endif(GUI)

include_directories(
../../
${CMAKE_SOURCE_DIR}
${CMAKE_BINARY_DIR}
${CMAKE_BINARY_DIR}/src
)

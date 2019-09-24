# Main Module Library
add_library(atomshakemodule
  core.cpp
  definition.cpp
  functions.cpp
  init.cpp
  process.cpp

  atomshake.h
)

# 'nogui' library
add_library(atomshakemodulenogui
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


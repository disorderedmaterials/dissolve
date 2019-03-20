# Main Module Library
add_library(optimisemodule
  core.cpp
  definition.cpp
  functions.cpp
  options.cpp
  process.cpp

  optimise.h
)

# 'nogui' library
add_library(optimisemodulenogui
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

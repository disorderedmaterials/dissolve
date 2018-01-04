# Main Module Library
add_library(energymodule
  core.cpp
  data.cpp
  definition.cpp
  functions.cpp
  method.cpp
  options.cpp

  energy.h
)

# 'nogui' library
add_library(energymodulenogui
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

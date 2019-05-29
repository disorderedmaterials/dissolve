# Main Module Library
add_library(geomoptmodule
  core.cpp
  definition.cpp
  functions.cpp
  options.cpp
  process.cpp

  geomopt.h
)

# 'nogui' library
add_library(geomoptmodulenogui
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

# Main Module Library
add_library(braggsqmodule
  core.cpp
  definition.cpp
  functions.cpp
  options.cpp
  process.cpp

  braggsq.h
)

# 'nogui' library
add_library(braggsqmodulenogui
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

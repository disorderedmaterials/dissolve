# Main Module Library
add_library(testmodule
  core.cpp
  definition.cpp
  functions.cpp
  options.cpp
  process.cpp

  test.h
)
# 'nogui' library
add_library(testmodulenogui
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

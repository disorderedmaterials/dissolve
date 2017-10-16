add_library(modules
  module.cpp
  modulelist.cpp
  moduleregistry.cpp
  
  module.h
  modulelist.h
  moduleregistry.h
)

include_directories(
../
${CMAKE_SOURCE_DIR}
${CMAKE_BINARY_DIR}
${CMAKE_BINARY_DIR}/src
)

# Module subdirectories
add_subdirectory(atomshake)
add_subdirectory(checks)
add_subdirectory(energy)
add_subdirectory(export)
add_subdirectory(forces)
add_subdirectory(import)
add_subdirectory(md)
add_subdirectory(partials)
add_subdirectory(skeleton)
add_subdirectory(test)

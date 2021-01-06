# Macros
macro(dissolve_add_forcefield ffname)
  get_filename_component(FF_DIR "${CMAKE_CURRENT_SOURCE_DIR}" NAME)
  message(STATUS "... Forcefield '${ffname}' in directory '${FF_DIR}'")

  # Create source file list
  set(FF_SRC "")
  file(
    GLOB FF_SRC
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    "*.cpp" "*.h"
  )
  message(STATUS "...    ${FF_SRC}")

  # Add library target
  add_library(ff_${ffname} ${FF_SRC})
  set(FF_LINK_LIBS
      ${FF_LINK_LIBS} "ff_${ffname}"
      CACHE INTERNAL "" FORCE
  )

  # Set common include dirs
  include_directories(ff_${ffname} PRIVATE ${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${CMAKE_BINARY_DIR}/src)
endmacro()

# Forcefield subdirectories
message(STATUS "Looking for forcefields in ${CMAKE_CURRENT_SOURCE_DIR}...")
file(
  GLOB children
  RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
  "*"
)
set(FF_DIRS "")
foreach(child ${children})
  if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${child} AND EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${child}/CMakeLists.txt)
    list(APPEND FF_DIRS ${child})
  endif()
endforeach()

foreach(subdir ${FF_DIRS})
  add_subdirectory(${subdir})
endforeach()

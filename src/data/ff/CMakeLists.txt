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
  target_include_directories(ff_${ffname} PRIVATE ${PROJECT_SOURCE_DIR}/src)
  target_link_libraries(ff_${ffname} PRIVATE base)
endmacro()

# Main Forcefield Library
add_library(
  ff
  angleTerm.cpp
  atomType.cpp
  bondTerm.cpp
  ff.cpp
  improperTerm.cpp
  library.cpp
  torsionTerm.cpp
  angleTerm.h
  atomType.h
  bondTerm.h
  ff.h
  improperTerm.h
  library.h
  torsionTerm.h
)

target_include_directories(ff PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(ff PRIVATE base)

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

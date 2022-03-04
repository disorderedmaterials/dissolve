# Macros
macro(dissolve_add_module header modulename)
  get_filename_component(MODULE_DIR "${CMAKE_CURRENT_SOURCE_DIR}" NAME)
  message(STATUS "... Module '${modulename}' in directory '${MODULE_DIR}'")

  # Create source file list
  set(MODULE_SRC "")
  file(
    GLOB MODULE_SRC
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    "*.cpp"
  )
  list(APPEND MODULE_SRC ${header})
  message(STATUS "...    ${MODULE_SRC}")

  # Add GUI component if requested and it exists
  if(GUI AND IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/gui)
    add_subdirectory(gui)
  endif()

  # Add library target
  add_library(module_${modulename} ${MODULE_SRC})
  set(MODULE_LINK_LIBS
      ${MODULE_LINK_LIBS} "module_${modulename}"
      CACHE INTERNAL "" FORCE
  )

  # Set common include dirs
  include_directories(
    module_${modulename}
    PRIVATE
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_BINARY_DIR}/src
    ${PROJECT_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}
    ${CMAKE_BINARY_DIR}/src
    ${CONAN_INCLUDE_DIRS_TBB}
  )
  target_link_libraries(
    module_${modulename}
    PRIVATE base
    PUBLIC ${THREADING_LINK_LIBS}
  )
endmacro()
macro(dissolve_add_module_gui modulename)
  message(STATUS "...    GUI component 'module_${modulename}_gui'")

  # Create source file lists
  set(MODULEGUI_MOC_HDRS "")
  file(
    GLOB MODULEGUI_MOC_HDRS
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    "*.h"
  )
  message(STATUS "...       ${MODULEGUI_MOC_HDRS}")
  set(MODULEGUI_UIS "")
  file(
    GLOB MODULEGUI_UIS
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    "*.ui"
  )
  message(STATUS "...       ${MODULEGUI_UIS}")
  set(MODULEGUI_SRC "")
  file(
    GLOB MODULEGUI_SRC
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    "*.cpp"
  )
  message(STATUS "...       ${MODULEGUI_SRC}")

  # Call Qt wrappers
  qt6_wrap_cpp(MODULEGUI_MOC_SRCS ${MODULEGUI_MOC_HDRS})
  qt6_wrap_ui(MODULEGUI_UIS_H ${MODULEGUI_UIS})

  # Add library target
  add_library(module_${modulename}_gui ${MODULEGUI_MOC_SRCS} ${MODULEGUI_UIS_H} ${MODULEGUI_SRC})
  set(MODULEGUI_LINK_LIBS
      ${MODULEGUI_LINK_LIBS} "module_${modulename}_gui"
      CACHE INTERNAL "" FORCE
  )

  # Because Qt has its own way of checking for C++17
  if(MSVC)
    target_compile_options(module_${modulename}_gui PUBLIC /Zc:__cplusplus)
  endif(MSVC)

  # Add dependency on main GUI components
  add_dependencies(module_${modulename}_gui gui)

  # Set common include dirs
  include_directories(
    module_${modulename}_gui
    PRIVATE
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_BINARY_DIR}/src
    ${Qt6Core_INCLUDE_DIRS}
    ${Qt6Gui_INCLUDE_DIRS}
    ${Qt6Widgets_INCLUDE_DIRS}
    ${Qt6OpenGL_INCLUDE_DIRS}
    ${Qt6OpenGLWidgets_INCLUDE_DIRS}
    ${FREETYPE_INCLUDE_DIRS}
  )
  target_link_libraries(
    module_${modulename}_gui
    PRIVATE base
    PUBLIC ${THREADING_LINK_LIBS}
  )
endmacro()

# Module subdirectories
message(STATUS "Looking for modules in ${CMAKE_CURRENT_SOURCE_DIR}...")
file(
  GLOB children
  RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
  "*"
)
set(MODULE_DIRS "")
foreach(child ${children})
  if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${child} AND EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${child}/CMakeLists.txt)
    list(APPEND MODULE_DIRS ${child})
  endif()
endforeach()

foreach(subdir ${MODULE_DIRS})
  add_subdirectory(${subdir})
endforeach()

add_library(moduleregistry registry.cpp registry.h)

include_directories(
  moduleregistry
  PRIVATE
  ${PROJECT_SOURCE_DIR}/src
  ${PROJECT_BINARY_DIR}/src
  ${PROJECT_SOURCE_DIR}
  ${CMAKE_BINARY_DIR}
  ${CMAKE_BINARY_DIR}/src
  ${CONAN_INCLUDE_DIRS}
)

if(GUI)
  add_library(modulewidgetproducer widget.cpp widgetproducer.cpp widget.h widgetproducer.h)
  add_dependencies(modulewidgetproducer ${MODULEGUI_LINK_LIBS})

  # Because Qt has its own way of checking for C++17
  if(MSVC)
    target_compile_options(modulewidgetproducer PUBLIC /Zc:__cplusplus)
  endif(MSVC)

  include_directories(
    modulewidgetproducer
    PRIVATE
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_BINARY_DIR}/src
    ${PROJECT_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}
    ${CMAKE_BINARY_DIR}/src
    ${Qt6Core_INCLUDE_DIRS}
    ${Qt6Gui_INCLUDE_DIRS}
    ${Qt6Widgets_INCLUDE_DIRS}
    ${Qt6OpenGL_INCLUDE_DIRS}
    ${Qt6OpenGLWidgets_INCLUDE_DIRS}
    ${FREETYPE_INCLUDE_DIRS}
    ${CONAN_INCLUDE_DIRS}
  )
endif(GUI)

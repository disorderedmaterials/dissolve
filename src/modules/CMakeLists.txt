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
  message(STATUS "...    Source files: ${MODULE_SRC}")

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
  target_include_directories(
    module_${modulename} PRIVATE ${PROJECT_SOURCE_DIR}/src ${PROJECT_BINARY_DIR}/src ${PROJECT_SOURCE_DIR} ${CMAKE_BINARY_DIR}
                                 ${CMAKE_BINARY_DIR}/src ${CONAN_INCLUDE_DIRS_TBB}
  )
  target_link_libraries(
    module_${modulename}
    PRIVATE base
    PUBLIC ${THREADING_LINK_LIBS}
  )
endmacro()
macro(dissolve_add_module_gui modulename)
  # Create source file lists
  set(MODULEGUI_H "")
  file(
    GLOB MODULEGUI_H
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    "*.h"
  )
  set(MODULEGUI_UI "")
  file(
    GLOB MODULEGUI_UI
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    "*.ui"
  )
  set(MODULEGUI_CPP "")
  file(
    GLOB MODULEGUI_CPP
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    "*.cpp"
  )

  message(STATUS "...    GUI component 'module_${modulename}_gui': ${MODULEGUI_CPP} ${MODULEGUI_H} ${MODULEGUI_UI}")

  # Add library target
  add_library(module_${modulename}_gui ${MODULEGUI_CPP} ${MODULEGUI_H} ${MODULEGUI_UI})
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
  target_include_directories(
    module_${modulename}_gui
    PRIVATE ${PROJECT_SOURCE_DIR}/src
            ${PROJECT_BINARY_DIR}/src
            ${CMAKE_BINARY_DIR}/src/gui/gui_autogen/include
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

target_include_directories(
  moduleregistry PRIVATE ${PROJECT_SOURCE_DIR}/src ${PROJECT_BINARY_DIR}/src ${PROJECT_SOURCE_DIR} ${CMAKE_BINARY_DIR}
                         ${CMAKE_BINARY_DIR}/src ${CONAN_INCLUDE_DIRS}
)

if(GUI)
  add_library(moduleWidgetProducer widget.cpp widgetProducer.cpp widget.h widgetProducer.h)
  add_dependencies(moduleWidgetProducer ${MODULEGUI_LINK_LIBS})

  # Because Qt has its own way of checking for C++17
  if(MSVC)
    target_compile_options(moduleWidgetProducer PUBLIC /Zc:__cplusplus)
  endif(MSVC)

  target_include_directories(
    moduleWidgetProducer
    PRIVATE ${PROJECT_SOURCE_DIR}/src
            ${PROJECT_BINARY_DIR}/src
            ${CMAKE_BINARY_DIR}/src/gui/gui_autogen/include
            ${Qt6Core_INCLUDE_DIRS}
            ${Qt6Gui_INCLUDE_DIRS}
            ${Qt6Widgets_INCLUDE_DIRS}
            ${Qt6OpenGL_INCLUDE_DIRS}
            ${Qt6OpenGLWidgets_INCLUDE_DIRS}
            ${FREETYPE_INCLUDE_DIRS}
            ${CONAN_INCLUDE_DIRS}
  )
endif(GUI)

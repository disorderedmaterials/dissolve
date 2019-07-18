project(Dissolve)

set(DESCRIPTION "Dissolve")
set(AUTHOR "Tristan Youngs")
set(VERSION_MAJOR "0")
set(VERSION_MINOR "4")
set(VERSION_PATCH "3")

set(CMAKE_BUILD_TYPE "Release")

cmake_minimum_required(VERSION 3.1)

if(COMMAND cmake_policy)
  cmake_policy(VERSION 2.6)
endif(COMMAND cmake_policy)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Make sure CMake won't try to generate rules for moc (we will do it ourselves)
set(CMAKE_AUTOMOC OFF)

# Set policy for automatic linkage of Qt libs to project
CMAKE_POLICY(SET CMP0020 NEW)

# Request C++11 Standard Support (Required for Qt 5.7)
set(CMAKE_CXX_STANDARD 11)

# Catch defines
if(PARALLEL)
  ADD_DEFINITIONS("-DPARALLEL")
  find_package(MPI REQUIRED)
  include_directories(${MPI_INCLUDE_PATH})
  set(EXTRA_LINK_LIBS ${EXTRA_LINK_LIBS} ${MPI_LIBRARIES})
endif(PARALLEL)

if(CHECKS)
  ADD_DEFINITIONS("-DCHECKS")
endif(CHECKS)

# Find basic requirements
include(FindBISON)
# -- Force first look for Bison to be /usr/local/bin, since version provided by XCode on OSX is too old (v2.3) 
find_package(BISON PATHS /usr/local/bin NO_DEFAULT_PATH QUIET)
find_package(BISON REQUIRED)

if(GUI)
  # Add extra functions
  LIST(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/Modules")
  include(FindFTGL)
  include(FindGLEXT)
  find_package(Qt5Widgets 5.4 REQUIRED)
  find_package(Qt5Core 5.4 REQUIRED)
  find_package(Qt5Gui 5.4 REQUIRED)
  find_package(Qt5PrintSupport 5.4 REQUIRED)
  find_package(OpenGL REQUIRED)
  find_package(FTGL REQUIRED)
  find_package(Freetype REQUIRED)
  find_package(GLEXT REQUIRED)

  # Make sure CMake won't try to generate rules for moc (we will do it ourselves)
  set(CMAKE_AUTOMOC OFF)

  # Set policy for automatic linkage of Qt libs to project
  CMAKE_POLICY(SET CMP0020 NEW)
endif(GUI)

# Perform system-specific setup
# -- Windows
if(WIN32)
  if(PARALLEL)
    set (target_name Dissolve)
  else(PARALLEL)
    set (target_name Dissolve-Serial)
    set (gui_target_name Dissolve-GUI)
    set (ed_target_name DGuidEd)
  endif(PARALLEL)

  # Adjust external include directories for GUI
  if(GUI)
    include_directories(
      ${FTGL_INCLUDE_DIRS}
      ${GLEXT_INCLUDE_DIRS}
    )
  endif(GUI)
 
  # Add global NOMINMAX define for Windows systems, and inhibit console creation
  ADD_DEFINITIONS("-DNOMINMAX")
  SET(GUI_TYPE WIN32)
  if(MINGW)
    SET(CMAKE_EXE_LINKER_FLAGS "-mwindows")
  endif(MINGW)
endif(WIN32)

# -- Unix
if(UNIX)
  if(PARALLEL)
    set (target_name dissolve)
  else(PARALLEL)
    set (target_name dissolve-serial)
    set (gui_target_name dissolve-gui)
    set (ed_target_name dguided)
  endif(PARALLEL)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
endif(UNIX)

# -- OSX
if(APPLE)
  # Set some specific C++11 related options here (set_property below does not seem to persist)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")

  if(PARALLEL)
    set (target_name dissolve)
  else(PARALLEL)
    set (target_name dissolve-serial)
    set (gui_target_name dissolve-gui)
    set (ed_target_name dguided)
  endif(PARALLEL)

  set(CMAKE_OSX_ARCHITECTURES "x86_64")
  ADD_DEFINITIONS(-D_MAC)
 
  if(GUI)
    # Find GLUT
    find_package(GLUT REQUIRED)
  endif(GUI)
endif(APPLE)

set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_BINDIR})
set(SRCS src/)

# Set bundle info
If(APPLE)
  set(MACOSX_BUNDLE_ICON_FILE  "Dissolve.icns")
  set(MACOSX_BUNDLE_GUI_IDENTIFIER  "Dissolve")
  set(MACOSX_BUNDLE_LONG_VERSION_STRING "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")
  set(MACOSX_BUNDLE_BUNDLE_NAME  "Dissolve" )
  set(MACOSX_BUNDLE_SHORT_VERSION_STRING "${VERSION_MAJOR}.${VERSION_MINOR}")
  set(MACOSX_BUNDLE_BUNDLE_VERSION  "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")
  set(MACOSX_BUNDLE_COPYRIGHT "${AUTHOR}")
endif(APPLE)

# Process CMakeLists in subdirs
add_subdirectory(${SRCS})

# Add executable target(s)
add_executable(${target_name}
  MACOSX_BUNDLE
  ${SRCS}/main.cpp 
)
# Set project-local include directories for target
target_include_directories(${target_name} PRIVATE
  ${PROJECT_SOURCE_DIR}/src
  ${PROJECT_BINARY_DIR}/src
  ${FREETYPE_INCLUDE_DIRS}
)

# Add GUI targets
if(GUI)

  # Main Dissolve GUI
  add_executable(${gui_target_name}
    MACOSX_BUNDLE
    ${SRCS}/dissolve-gui.cpp 
  )

  # Set project-local include directories for target
  target_include_directories(${gui_target_name} PRIVATE
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_BINARY_DIR}/src
    ${FREETYPE_INCLUDE_DIRS}
    ${Qt5Core_INCLUDE_DIRS}
    ${Qt5Gui_INCLUDE_DIRS}
    ${Qt5OpenGL_INCLUDE_DIRS}
    ${Qt5Widgets_INCLUDE_DIRS}
  )

  # Dissolve Guide Editor (dguided)
  add_executable(${ed_target_name}
    MACOSX_BUNDLE
    ${SRCS}/dguided.cpp 
  )

  # Set project-local include directories for target
  target_include_directories(${ed_target_name} PRIVATE
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_BINARY_DIR}/src
    ${FREETYPE_INCLUDE_DIRS}
    ${Qt5Core_INCLUDE_DIRS}
    ${Qt5Gui_INCLUDE_DIRS}
    ${Qt5Widgets_INCLUDE_DIRS}
  )

endif(GUI)

# Set basic link libs for executables
set(BASIC_LINK_LIBS

  # Main libs
  main classes module export import procedure procedurenodes keywordtypes expression genericitems base math data ff 

  # Modules
  analysemodule
  atomshakemodule
  braggsqmodule
  calculatedanglemodule
  calculaterdfmodule
  calibratemodule
  checksmodule
  datatestmodule
  energymodule
  epsrmodule
  exportmodule
  forcesmodule
  geomoptmodule
  importmodule
  intrashakemodule
  mdmodule
  molshakemodule
  neutronsqmodule
  rdfmodule
  refinemodule
  sanitycheckmodule
  skeletonmodule
  sqmodule
  testmodule
)

# Set linker options for complete/no archive
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set(WHOLE_ARCHIVE_FLAG "-Wl,-all_load")
  set(NO_WHOLE_ARCHIVE_FLAG "-Wl,-noall_load")
else()
  set(WHOLE_ARCHIVE_FLAG "-Wl,-whole-archive")
  set(NO_WHOLE_ARCHIVE_FLAG "-Wl,-no-whole-archive")
endif()

# Build CLI target executable
target_link_libraries( ${target_name}
  ${WHOLE_ARCHIVE_FLAG}

  ${BASIC_LINK_LIBS}

  # Module 'nogui' libs
  analysemodulenogui
  atomshakemodulenogui
  braggsqmodulenogui
  calculatedanglemodulenogui
  calculaterdfmodulenogui
  calibratemodulenogui
  datatestmodulenogui
  energymodulenogui
  epsrmodulenogui
  geomoptmodulenogui
  neutronsqmodulenogui
  rdfmodulenogui
  refinemodulenogui
  skeletonmodulenogui
  sqmodulenogui
  testmodulenogui
  ${NO_WHOLE_ARCHIVE_FLAG}

  ${EXTRA_LINK_LIBS}
)

# Build GUI target executables
if(GUI)

  # Main Dissolve GUI
  target_link_libraries( ${gui_target_name}
    ${WHOLE_ARCHIVE_FLAG}

    ${BASIC_LINK_LIBS}

    # Module gui libs
    analysemodulegui
    atomshakemodulegui
    braggsqmodulegui
    calculatedanglemodulegui
    calculaterdfmodulegui
    calibratemodulegui
    datatestmodulegui
    energymodulegui
    epsrmodulegui
    geomoptmodulegui
    neutronsqmodulegui
    rdfmodulegui
    refinemodulegui
    skeletonmodulegui
    sqmodulegui
    testmodulegui

    # Main gui libs
    render
    viewer
    widgets
    keywordwidgets
    delegates
    gui

    ${NO_WHOLE_ARCHIVE_FLAG}

    # External libs
    Qt5::Widgets Qt5::Core Qt5::PrintSupport
    ${FTGL_LIBRARIES}
    ${OPENGL_LIBRARIES}
    ${FREETYPE_LIBRARIES}

    ${EXTRA_LINK_LIBS}
  )

  # Dissolve Guide Editor
  target_link_libraries( ${ed_target_name}
    ${WHOLE_ARCHIVE_FLAG}

    ${BASIC_LINK_LIBS}

    # Module gui libs
    analysemodulegui
    atomshakemodulegui
    braggsqmodulegui
    calculatedanglemodulegui
    calculaterdfmodulegui
    calibratemodulegui
    datatestmodulegui
    energymodulegui
    epsrmodulegui
    geomoptmodulegui
    neutronsqmodulegui
    rdfmodulegui
    refinemodulegui
    skeletonmodulegui
    sqmodulegui
    testmodulegui

    # Main gui libs
    render
    viewer
    widgets
    keywordwidgets
    delegates
    gui
    ed

    ${NO_WHOLE_ARCHIVE_FLAG}

    # External libs
    Qt5::Widgets Qt5::Core Qt5::PrintSupport
    ${FTGL_LIBRARIES}
    ${OPENGL_LIBRARIES}
    ${FREETYPE_LIBRARIES}

    ${EXTRA_LINK_LIBS}
  )
endif(GUI)

# Install target for unix
#if(UNIX AND NOT APPLE)
#  INSTALL_TARGET(${target_name} "")
#  configure_file(${PROJECT_SOURCE_DIR}/desktop.cmake Dissolve.desktop)
#  install(FILES ${PROJECT_SOURCE_DIR}/src/gui/icons/dissolve.png
#        DESTINATION share/dissolve)
#  install(FILES ${PROJECT_BINARY_DIR}/Dissolve.desktop
#        DESTINATION share/applications)
#  foreach(i ff filters fftesting fragments partitions scripts test includes)
#    install(DIRECTORY ${PROJECT_SOURCE_DIR}/data/${i}
#          DESTINATION share/dissolve)
#  endforeach(i)
#endif(UNIX AND NOT APPLE)


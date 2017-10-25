project(dUQ)

set(DESCRIPTION "dUQ")
set(AUTHOR "Tristan Youngs")
set(VERSION_MAJOR "0")
set(VERSION_MINOR "33")
set(VERSION_PATCH "3")

set(CMAKE_BUILD_TYPE "Release")

cmake_minimum_required(VERSION 2.8.12)

if(COMMAND cmake_policy)
  cmake_policy(VERSION 2.6)
endif(COMMAND cmake_policy)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Make sure CMake won't try to generate rules for moc (we will do it ourselves)
set(CMAKE_AUTOMOC OFF)

# Set policy for automatic linkage of Qt libs to project
CMAKE_POLICY(SET CMP0020 NEW)

# Perform system-specific setup
# -- Windows
if(WIN32)
  set (target_name dUQ)
 
  # Add global NOMINMAX define for Windows systems, and inhibit console creation
  ADD_DEFINITIONS("-DNOMINMAX")
  SET(GUI_TYPE WIN32)
  if(MINGW)
    SET(CMAKE_EXE_LINKER_FLAGS "-mwindows")
  endif(MINGW)
endif(WIN32)

# -- Unix
if(UNIX)
  set (target_name duq)
endif(UNIX)

# -- OSX
if(APPLE)
  set (target_name duq)
  set(CMAKE_OSX_ARCHITECTURES "x86_64")
  ADD_DEFINITIONS(-D_MAC)
endif(APPLE)

set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_BINDIR})
set(SRCS src/)

# Set bundle info
If(APPLE)
  set(MACOSX_BUNDLE_ICON_FILE  "dUQ.icns")
  set(MACOSX_BUNDLE_GUI_IDENTIFIER  "dUQ")
  set(MACOSX_BUNDLE_LONG_VERSION_STRING "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")
  set(MACOSX_BUNDLE_BUNDLE_NAME  "dUQ" )
  set(MACOSX_BUNDLE_SHORT_VERSION_STRING "${VERSION_MAJOR}.${VERSION_MINOR}")
  set(MACOSX_BUNDLE_BUNDLE_VERSION  "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")
  set(MACOSX_BUNDLE_COPYRIGHT "${AUTHOR}")
endif(APPLE)

# Process CMakeLists in subdirs
add_subdirectory(${SRCS})

add_executable(${target_name}
  MACOSX_BUNDLE
  ${SRCS}/main.cpp 
)

# Set project-local include directories for target
target_include_directories(${target_name} PRIVATE
  ${PROJECT_SOURCE_DIR}/src
  ${PROJECT_BINARY_DIR}/src
  ${FREETYPE_INCLUDE_DIRS}
  ${HDF5_INCLUDE_DIRS}
)

# Perform final link
target_link_libraries( ${target_name}
  main modules classes base math
)

# Install target for unix
#if(UNIX AND NOT APPLE)
#  INSTALL_TARGET(${target_name} "")
#  configure_file(${PROJECT_SOURCE_DIR}/desktop.cmake dUQ.desktop)
#  install(FILES ${PROJECT_SOURCE_DIR}/src/gui/icons/duq.png
#        DESTINATION share/duq)
#  install(FILES ${PROJECT_BINARY_DIR}/dUQ.desktop
#        DESTINATION share/applications)
#  foreach(i ff filters fftesting fragments partitions scripts test includes)
#    install(DIRECTORY ${PROJECT_SOURCE_DIR}/data/${i}
#          DESTINATION share/duq)
#  endforeach(i)
#endif(UNIX AND NOT APPLE)


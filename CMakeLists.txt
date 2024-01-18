cmake_minimum_required(VERSION 3.15)
project(Dissolve)

set(DESCRIPTION "Dissolve")
set(AUTHOR "Team Dissolve")
set(VERSION_MAJOR "1")
set(VERSION_MINOR "4")
set(VERSION_PATCH "2")

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
endif()

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Make sure CMake won't try to generate rules for moc (we will do it ourselves)
set(CMAKE_AUTOMOC ON)

# Set policy for automatic linkage of Qt libs to project
cmake_policy(SET CMP0020 NEW)

# Request C++17 Standard Support (Required for std::optional and auto types in lambda parameters)
set(CMAKE_CXX_STANDARD 17)

# Include Conan Packages
option(CONAN "Use conan to find dependencies" ON)
if(CONAN)
  include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
  conan_basic_setup(TARGETS)
  include_directories(${CONAN_INCLUDE_DIRS_TOML11})
  include_directories(${CONAN_INCLUDE_DIRS_FMT})
else(CONAN)
  find_package(fmt REQUIRED)
  find_package(CLI11 REQUIRED)
  find_package(toml11 REQUIRED)
  find_package(pugixml REQUIRED)
  include_directories(${AntlrRuntime_INCLUDE_DIRS})
  link_directories(${AntlrRuntime_LINK_DIRS})
endif(CONAN)

add_compile_definitions(FMT_HEADER_ONLY)

# Catch defines
if(PARALLEL)
  add_definitions("-DPARALLEL")
  find_package(MPI REQUIRED)
  include_directories(${MPI_INCLUDE_PATH})
  set(EXTRA_LINK_LIBS ${EXTRA_LINK_LIBS} ${MPI_LIBRARIES})
endif(PARALLEL)

# Build with threads
option(MULTI_THREADING "Enable threading using tbb" ON)
if(MULTI_THREADING)
  add_definitions("-DMULTITHREADING")
  if(CONAN)
    include_directories(CONAN_INCLUDE_DIRS_ONETBB)
    include_directories(CONAN_INCLUDE_DIRS_ONEDPL)
    set(THREADING_LINK_LIBS ${THREADING_LINK_LIBS} CONAN_PKG::onetbb CONAN_PKG::onedpl)
  else(CONAN)
    set(THREADING_LINK_LIBS ${THREADING_LINK_LIBS} $ENV{THREADING_LINK_LIBS})
  endif(CONAN)
  message("Threading Link Libs: ${THREADING_LINK_LIBS}")
endif(MULTI_THREADING)

# Add local Modules dir to cmake search path
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/Modules")

# Find ANTLR4 Java
include(FindANTLR)
find_package(ANTLR REQUIRED)

if(GUI)
  # Add extra functions
  include(FindFTGL)

  if(APPLE)
    set(CMAKE_PREFIX_PATH "${QT_BASE_DIR}")
  endif(APPLE)

  find_package(OpenGL REQUIRED)
  find_package(
    Qt6
    COMPONENTS Core
               DBusTools
               Gui
               Widgets
               OpenGL
               OpenGLWidgets
               Qml
               QmlModels
               QmlTools
               Quick
               QuickWidgets
               QuickControls2
               REQUIRED
  )
  find_package(FTGL REQUIRED)
  find_package(Freetype REQUIRED)
  add_definitions(-DQT_NO_KEYWORDS)

  # Make sure CMake won't try to generate rules for moc (we will do it ourselves)
  set(CMAKE_AUTOMOC ON)

  # Set policy for automatic linkage of Qt libs to project
  cmake_policy(SET CMP0020 NEW)
endif(GUI)

# Perform system-specific setup -- Windows
if(WIN32)
  if(PARALLEL)
    set(target_name Dissolve-MPI)
  else(PARALLEL)
    set(target_name Dissolve)
    set(gui_target_name Dissolve-GUI)
  endif(PARALLEL)

  # Adjust external include directories for GUI
  if(GUI)
    include_directories(${FTGL_INCLUDE_DIRS})
  endif(GUI)

  # Add defines for Windows systems - NOMINMAX to prevent conflicts with std::min() and std::max() - NOGDI to prevent conflicts arising from
  # Windows defining ERROR as a global macro (used in ANTLR4)
  add_definitions(-DNOMINMAX -DNOGDI)
  # Needed for Qt 6.4
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /permissive-")
endif(WIN32)

# -- Unix
if(UNIX)
  if(PARALLEL)
    set(target_name dissolve-mpi)
  else(PARALLEL)
    set(target_name dissolve)
    set(gui_target_name dissolve-gui)
  endif(PARALLEL)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
endif(UNIX)

# -- OSX
if(APPLE)
  # Set some specific C++11 related options here (set_property below does not seem to persist)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")

  # Suppress warnings for undefined template vars
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-undefined-var-template")

  if(PARALLEL)
    set(target_name dissolve-mpi)
  else(PARALLEL)
    set(target_name dissolve)
    set(gui_target_name dissolve-gui)
  endif(PARALLEL)

  if(NOT CMAKE_OSX_ARCHITECTURES)
    set(CMAKE_OSX_ARCHITECTURES "x86_64")
  endif(CMAKE_OSX_ARCHITECTURES)
  add_definitions(-D_MAC)

  if(MULTI_THREADING)
    set(THREADING_LINK_LIBS ${THREADING_LINK_LIBS} CONAN_PKG::onedpl)
  endif(MULTI_THREADING)

  if(GUI)
    # Find GLUT
    find_package(GLUT REQUIRED)

    # Adjust external include directories for GUI
    include_directories(${FTGL_INCLUDE_DIRS})
  endif(GUI)
endif(APPLE)

set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_BINDIR})
set(SRCS src/)

# Set bundle info
if(APPLE)
  set(MACOSX_BUNDLE_ICON_FILE "Dissolve.icns")
  set(MACOSX_BUNDLE_GUI_IDENTIFIER "Dissolve")
  set(MACOSX_BUNDLE_LONG_VERSION_STRING "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")
  set(MACOSX_BUNDLE_BUNDLE_NAME "Dissolve")
  set(MACOSX_BUNDLE_SHORT_VERSION_STRING "${VERSION_MAJOR}.${VERSION_MINOR}")
  set(MACOSX_BUNDLE_BUNDLE_VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")
  set(MACOSX_BUNDLE_COPYRIGHT "${AUTHOR}")
endif(APPLE)

# Process CMakeLists in subdirs
set(MODULE_LINK_LIBS
    ""
    CACHE INTERNAL ""
)
set(MODULEGUI_LINK_LIBS
    ""
    CACHE INTERNAL ""
)
set(MODULENOGUI_LINK_LIBS
    ""
    CACHE INTERNAL ""
)
set(FF_LINK_LIBS
    ""
    CACHE INTERNAL ""
)
add_subdirectory(${SRCS})

# Add executable target(s)
add_executable(${target_name} MACOSX_BUNDLE ${SRCS}/main.cpp)

# Set project-local include directories for target
target_include_directories(
  ${target_name} PRIVATE ${PROJECT_SOURCE_DIR}/src ${PROJECT_BINARY_DIR}/src ${FREETYPE_INCLUDE_DIRS} ${ANTLR4_INCLUDE_DIRS}
                         ${ANTLR_OUTPUT_DIRS}
)

# Add GUI targets
if(GUI)

  # Main Dissolve GUI
  qt_add_executable(${gui_target_name} MACOSX_BUNDLE ${SRCS}/dissolve-gui.cpp)

  # Set project-local include directories for target
  target_include_directories(
    ${gui_target_name}
    PRIVATE ${PROJECT_SOURCE_DIR}/src
            ${PROJECT_BINARY_DIR}/src
            ${FREETYPE_INCLUDE_DIRS}
            ${Qt6Core_INCLUDE_DIRS}
            ${Qt6Gui_INCLUDE_DIRS}
            ${Qt6OpenGL_INCLUDE_DIRS}
            ${Qt6Widgets_INCLUDE_DIRS}
            ${Qt6OpenGL_INCLUDE_DIRS}
            ${Qt6OpenGLWidgets_INCLUDE_DIRS}
  )

endif(GUI)

# Set basic link libs for executables
set(BASIC_LINK_LIBS
    # Main libs
    main
    classes
    kernels
    potentials
    module
    moduleregistry
    io
    export
    import
    neta
    analyser
    procedure
    procedureNodes
    keywords
    expression
    items
    base
    math
    data
    sginfo
    # Forcefields
    ff
    ${FF_LINK_LIBS}
    # Modules
    ${MODULE_LINK_LIBS}
)

# Set linker options for complete/no archive
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set(WHOLE_ARCHIVE_FLAG "-Wl,-all_load")
else()
  set(WHOLE_ARCHIVE_FLAG "-Wl,-whole-archive")
  set(NO_WHOLE_ARCHIVE_FLAG "-Wl,-no-whole-archive")
endif()

# Enable build of test suite?
option(BUILD_TESTS "Build test suite" OFF)
if(BUILD_TESTS)
  enable_testing()
  add_subdirectory(examples)
  add_subdirectory(tests)
endif(BUILD_TESTS)

if(BUILD_BENCHMARKS)
  # The conan package of goooglebenchmark segfaults - so lets use fetch content
  include(FetchContent)
  set(BENCHMARK_ENABLE_TESTING "OFF")
  fetchcontent_declare(
    googlebenchmark
    GIT_REPOSITORY https://github.com/google/benchmark.git
    GIT_TAG v1.5.4
  )

  fetchcontent_makeavailable(googlebenchmark)
  add_subdirectory(benchmark)
endif(BUILD_BENCHMARKS)

# Build CLI target executable
target_link_libraries(
  ${target_name}
  PUBLIC ${WHOLE_ARCHIVE_FLAG} ${BASIC_LINK_LIBS} ${NO_WHOLE_ARCHIVE_FLAG}
  PRIVATE # External libs
          ${EXTRA_LINK_LIBS} ${THREADING_LINK_LIBS}
)

# Build GUI target executables
if(GUI)

  # Main Dissolve GUI
  target_link_libraries(
    ${gui_target_name}
    PUBLIC ${WHOLE_ARCHIVE_FLAG}
           ${BASIC_LINK_LIBS}
           # Module gui libs
           ${MODULEGUI_LINK_LIBS}
           moduleWidgetProducer
           # Main gui libs
           render
           widgets
           keywordWidgets
           delegates
           models
           gui
           ${NO_WHOLE_ARCHIVE_FLAG}
    PRIVATE # External libs
            Qt6::Widgets
            Qt6::Core
            OpenGL::GL
            ${FTGL_LIBRARIES}
            ${FREETYPE_LIBRARIES}
            ${EXTRA_LINK_LIBS}
            ${THREADING_LINK_LIBS}
  )
endif(GUI)

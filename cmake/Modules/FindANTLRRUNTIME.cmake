# FindANTLRRuntime.cmake

# Find runtime library and headers, and define:
#  ANTLRRUNTIME_INCLUDE_DIRS    -   Location of runtime dev headers
#  ANTLRRUNTIME_LIBRARIES       -   Link libs

FIND_PATH(ANTLRRUNTIME_INCLUDE_DIR antlr4-runtime.h
  HINTS
  ${ANTLRRUNTIME_DIR}/include
  PATH_SUFFIXES antlr4-runtime include
  PATHS
  /usr
  /usr/local
  /sw
  /opt/local
  /usr/freeware
  /usr/local/Cellar
)
message(STATUS "ANTLR4 runtime main header dir = ${ANTLRRUNTIME_INCLUDE_DIR}")

FIND_LIBRARY(ANTLRRUNTIME_LIBRARY
  NAMES antlr4-runtime
  HINTS
  ${ANTLRRUNTIME_DIR}
  PATH_SUFFIXES lib64 lib
  PATHS
  /usr
  /usr/local
  /usr/local/Cellar
  /sw
  /usr/freeware
)
message(STATUS "ANTLR4 runtime library = ${ANTLRRUNTIME_LIBRARY}")

get_filename_component(ANTLRRUNTIME_LIBRARIES_PATH ${ANTLRRUNTIME_LIBRARY} DIRECTORY)
message(STATUS "ANTLR4 runtime library path = ${ANTLRRUNTIME_LIBRARIES_PATH}")

# set the user variables
IF(ANTLRRUNTIME_INCLUDE_DIR)
  SET(ANTLRRUNTIME_INCLUDE_DIRS "${ANTLRRUNTIME_INCLUDE_DIR}")
ENDIF()
SET(ANTLRRUNTIME_LIBRARIES "${ANTLRRUNTIME_LIBRARY}")

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    ANTLRRUNTIME
    DEFAULT_MSG  ANTLRRUNTIME_INCLUDE_DIRS  ANTLRRUNTIME_LIBRARIES)

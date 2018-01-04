# - Locate readline library and headers
# This module defines
#  READLINE_LIBRARIES, the library to link against
#  READLINE_FOUND, if false, do not try to link to readline
#  READLINE_INCLUDE_DIRS, where to find headers.
#
# $READLINE_DIR is an environment variable that points to the directory in which the readline lib resides.

FIND_PATH(READLINE_INCLUDE_DIR readline/readline.h 
  HINTS
  $ENV{READLINE_DIR}
  PATH_SUFFIXES include src
  PATHS
  /usr/include
  /usr/local/include
  /sw/include
  /opt/local/include
  /usr/freeware/include
)

FIND_LIBRARY(READLINE_LIBRARY
  NAMES readline libreadline readline_static
  HINTS
  $ENV{READLINE_DIR}
  PATH_SUFFIXES lib64 lib
  PATHS
  /usr/lib
  /usr/local/lib
  /sw
  /usr/freeware
)

# set the user variables
IF(READLINE_INCLUDE_DIR)
  SET(READLINE_INCLUDE_DIRS "${READLINE_INCLUDE_DIR}")
ENDIF()
SET(READLINE_LIBRARIES "${READLINE_LIBRARY}")

# handle the QUIETLY and REQUIRED arguments and set READLINE_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE("FindPackageHandleStandardArgs")
FIND_PACKAGE_HANDLE_STANDARD_ARGS(READLINE  DEFAULT_MSG  READLINE_INCLUDE_DIR  READLINE_LIBRARY)

IF (READLINE_FOUND)
	MESSAGE(STATUS "Found READLINE: ${READLINE_LIBRARIES}")
ENDIF (READLINE_FOUND)

MARK_AS_ADVANCED(READLINE_INCLUDE_DIR)

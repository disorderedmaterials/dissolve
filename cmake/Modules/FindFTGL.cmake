# - Locate FTGL library
# This module defines
#  FTGL_LIBRARIES, the library to link against
#  FTGL_FOUND, if false, do not try to link to FTGL
#  FTGL_INCLUDE_DIRS, where to find headers.
#
# $FTGL_DIR is an environment variable that points to the main ftgl directory.

#=============================================================================
# Copyright 2011 Richard Ulrich.
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================

# Created by Richard Ulrich.
FIND_PATH(FTGL_INCLUDE_DIR FTGL/ftgl.h 
  HINTS
  $ENV{FTGL_DIR}
  PATH_SUFFIXES include src
  PATHS
  /usr/include
  /usr/local/include
  /sw/include
  /opt/local/include
  /usr/freeware/include
)

FIND_LIBRARY(FTGL_LIBRARY
  NAMES ftgl libftgl ftgl_static
  HINTS
  $ENV{FTGL_DIR}
  PATH_SUFFIXES lib64 lib
  PATHS
  /usr/lib
  /usr/local/lib
  /sw
  /usr/freeware
)

  # set the user variables
IF(FTGL_INCLUDE_DIR)
  SET(FTGL_INCLUDE_DIRS "${FTGL_INCLUDE_DIR}")
ENDIF()
SET(FTGL_LIBRARIES "${FTGL_LIBRARY}")

 # handle the QUIETLY and REQUIRED arguments and set FTGL_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE("FindPackageHandleStandardArgs")
FIND_PACKAGE_HANDLE_STANDARD_ARGS(FTGL  DEFAULT_MSG  FTGL_LIBRARY  FTGL_INCLUDE_DIR)

 MARK_AS_ADVANCED(FTGL_LIBRARY FTGL_INCLUDE_DIR)

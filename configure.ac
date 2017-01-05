#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
AC_PREREQ(2.60)

# Set program name, version and source directory
m4_define([DUQ_VERSION],[0.32.8])
AC_INIT([duq], DUQ_VERSION)
AC_CONFIG_AUX_DIR(config)
AC_CANONICAL_TARGET
AC_CONFIG_SRCDIR([src/main.cpp])
AC_CONFIG_HEADER(config.h)
AM_INIT_AUTOMAKE


# Set initial variable values
PARALLEL_CFLAGS=""
CHECK_CFLAGS=""


# Determine C++ compiler - Parallel Build?
AC_ARG_WITH([parallel], [AC_HELP_STRING([--with-parallel], [Request parallel code (requires mpic++ in path) [default="no"]])], [with_parallel="yes"], [with_parallel="no"])
if test "$with_parallel" = "yes"; then
  AC_DEFINE([PARALLEL], [1], [Parallel code enabled])
  if test "x$CXX" = "x"; then
    CXX="mpic++"
  fi
  PARALLEL_CFLAGS="-DPARALLEL"
fi


# Check for suitable compiler (for MPI builds)
AC_PROG_CXX()
if test "x$CXX" = "x"; then
  AC_MSG_ERROR([Parallel build requested, but no mpic++ found in path.], [1])
fi


# Check other basic requirements
AC_PROG_INSTALL
AC_PROG_LIBTOOL


# Compile with internal object checking for debugging / testing?
AC_ARG_WITH([checks], [AC_HELP_STRING([--with-checks], [Enable internal code array/pointer checking [default="no"]])], [with_checks="yes"], [with_checks="no"])
if test "$with_checks" = "yes"; then
  CHECK_CFLAGS="-DCHECKS"
fi


# Readline include dir
RLINCDIR=""
AC_ARG_WITH([readline-includes],[AC_HELP_STRING([--with-readline-includes=<dir>],
    [Specify location of readline include files [default="/usr/include/readline"]])], ,
    [with_readline_includes="/usr/include/readline"])
RLINCDIR="$with_readline_includes"
AC_CHECK_HEADER([$RLINCDIR/readline.h],,[AC_MSG_ERROR([Cannot find headers for readline.])])


# Build Directory (used when packaging)
BUILDDIR=""
AC_ARG_WITH([build-dir],[AC_HELP_STRING([--with-build-dir=<dir>],
    [Specify location of build environment when packaging [default=""]])], ,
    [with_build_dir=""])
BUILDDIR="$with_build_dir"


# Install Directory (used when packaging - should be '$PREFIX - $BUILDDIR')
INSTALLDIR="/usr/local"
AC_ARG_WITH([install-dir],[AC_HELP_STRING([--with-install-dir=<dir>],
    [Specify 'clean' install dir prefix, i.e. without any BUILDDIR [default="/usr/local"]])], ,
    [with_install_dir="/usr/local"])
# If --prefix has been give, use this value by default
if test "$prefix" != "NONE"; then
  INSTALLDIR="$prefix"
fi
if test "$with_install_dir" != "/usr/local"; then
  INSTALLDIR="$with_install_dir"
fi


# Pre-initialise pkg-config
PKG_PROG_PKG_CONFIG


# Perform Makefile substitutions
AC_SUBST(MPI_LDLIBS)
AC_SUBST(BUILDDIR)
AC_SUBST(INSTALLDIR)
VERSION=DUQ_VERSION
AC_SUBST(VERSION)
DUQ_CFLAGS="$PARALLEL_CFLAGS $CHECK_CFLAGS"
AC_SUBST(DUQ_CFLAGS)
DUQ_READLINE="-L$RLINCDIR"
AC_SUBST(DUQ_READLINE)


# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_STRUCT_TM


# Checks for library functions.
AC_FUNC_SELECT_ARGTYPES
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([floor pow select sqrt strchr strrchr strstr])


# Define Makefiles in AC_OUTPUT 
AC_CONFIG_FILES(Makefile)
AC_CONFIG_FILES(src/Makefile)
AC_CONFIG_FILES(src/base/Makefile)
AC_CONFIG_FILES(src/classes/Makefile)
AC_CONFIG_FILES(src/main/Makefile)
AC_CONFIG_FILES(src/math/Makefile)
AC_CONFIG_FILES(src/modules/Makefile)
AC_CONFIG_FILES(src/templates/Makefile)
AC_OUTPUT


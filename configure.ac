#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
AC_PREREQ(2.60)

# Set program name, version, bug-address and source directory
m4_define([DUQ_VERSION],[r4])
AC_INIT(duq,DUQ_VERSION,tris@duq.net)
AC_CONFIG_SRCDIR([src/])

# Check system type and set defines
# Specify a single file in the source tree so we can check that --srcdir points to the right place
AC_CONFIG_AUX_DIR(config)
AC_CONFIG_HEADER(config.h)
AC_CANONICAL_TARGET
MAC_LIBS=""
if test "$target_vendor" = "apple"; then
  AC_DEFINE([IS_MAC], [], [Whether operating system is an Apple])
  MAC_LIBS="-framework OpenGL"
fi
AC_SUBST(MAC_LIBS)

AM_INIT_AUTOMAKE(duq,DUQ_VERSION)


# Set initial variable values
PARALLEL_CFLAGS=""
CHECK_CFLAGS=""
GUI_SUBDIR=""


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

# GUI Build
AC_ARG_WITH([gui], [AC_HELP_STRING([--with-gui], [Request GUI generation (requires Qt4) [default="no"]])], [with_gui="yes"], [with_gui="no"])
if test "$with_gui" = "yes"; then
  GUI_SUBDIR="gui"
fi

# Checks?
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

# Qt4 Tools
QTMOC="moc"
QTRCC="rcc"
QTUIC="uic"
AC_ARG_WITH([qtmoc],[AC_HELP_STRING([--with-qtmoc=<executable>],
    [Specify Qt4 Meta-Object Compiler tool (moc) [default="moc"]])], ,
    [with_qtmoc="moc"])
AC_ARG_WITH([qtuic],[AC_HELP_STRING([--with-qtuic=<executable>],
    [Specify Qt4 User Interface Compiler tool (uic) [default="uic"]])], ,
    [with_qtuic="uic"])
AC_ARG_WITH([qtrcc],[AC_HELP_STRING([--with-qtrcc=<executable],
    [Specify Qt4 Resource Compiler tool (rcc) [default="rcc"]])], ,
    [with_qtrcc="rcc"])
QTMOC="$with_qtmoc"
QTRCC="$with_qtrcc"
QTUIC="$with_qtuic"

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


# QT-FRAMEWORK  (Mac only)
AC_ARG_WITH([qt],[AC_HELP_STRING([--with-qt=framework|fink|<none>],
    [Determines which installation of Qt4 to use on OS X (should not be set for Linux)[default=<none>]])], ,
    [with_qt=])


# Pre-initialise pkg-config  (unless using Qt framework on the Mac)
if test "$with_qt" != "framework"; then
  PKG_PROG_PKG_CONFIG
fi


# Find Qt (if building GUI)
if test "$with_gui" = "yes"; then
  if test "$with_qt" = "framework"; then
    QTGUI_LIBS="-framework QtGui -framework QtOpenGL -framework QtCore"
    QTGUI_INCLUDES="-F QtGui -F QtOpenGl -F QtCore"
  fi
  if test "$with_qt" = "fink"; then
    QTGUI_LIBS="-L/sw/lib/qt4-x11/lib -lQtGui -lQtCore -lQtOpenGL"
    QTGUI_INCLUDES="-I/sw/lib/qt4-x11/include -I/usr/X11R6/include"
    QTOPENGL_CFLAGS="-I/sw/lib/qt4-x11/include"
    QTOPENGL_LIBS="-I/sw/lib/qt4-x11/lib"
    PKG_CHECK_MODULES(QTGUI, QtGui >= 4.1.0)
    PKG_CHECK_MODULES(QTOPENGL, QtOpenGL >= 4.1.0)
  fi
  if test "x$with_qt" = "x"; then
    PKG_CHECK_MODULES(QTGUI, QtGui >= 4.1.0)
    PKG_CHECK_MODULES(QTOPENGL, QtOpenGL >= 4.1.0)
  fi

  # Perform Makefile substitutions
  GUI_LDLIBS="$QTGUI_LIBS $QTOPENGL_LIBS -lncurses -lGL"
  GUI_INCLUDES="$QTGUI_INCLUDES"
  AC_SUBST(GUI_LDLIBS)
  AC_SUBST(GUI_INCLUDES)
  AC_SUBST(QTMOC)
  AC_SUBST(QTUIC)
  AC_SUBST(QTRCC)
fi


# Perform Makefile substitutions
AC_SUBST(MPI_LDLIBS)
AC_SUBST(BUILDDIR)
AC_SUBST(INSTALLDIR)
VERSION=DUQ_VERSION
AC_SUBST(VERSION)
DUQ_CFLAGS="$PARALLEL_CFLAGS $CHECK_CFLAGS"
AC_SUBST(DUQ_CFLAGS)
AC_SUBST(GUI_SUBDIR)
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
AC_CONFIG_FILES(src/cli/Makefile)
AC_CONFIG_FILES(src/lib/Makefile)
AC_CONFIG_FILES(src/lib/base/Makefile)
AC_CONFIG_FILES(src/lib/classes/Makefile)
AC_CONFIG_FILES(src/lib/main/Makefile)
AC_CONFIG_FILES(src/lib/math/Makefile)
AC_CONFIG_FILES(src/lib/templates/Makefile)
if test "$with_gui" = "yes"; then
  AC_CONFIG_FILES(src/gui/Makefile)
fi
AC_OUTPUT


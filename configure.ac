#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
AC_PREREQ(2.60)

# Set program name, version and source directory
m4_define([DISSOLVE_VERSION],[0.36.0])
AC_INIT([dissolve], DISSOLVE_VERSION)
AC_CONFIG_AUX_DIR(config)
AC_CANONICAL_TARGET
AC_CONFIG_SRCDIR([src/main.cpp])
AC_CONFIG_HEADER(config.h)
AM_INIT_AUTOMAKE


# Set initial variable values
PARALLEL_CFLAGS=""
CHECK_CFLAGS=""
GUI_CFLAGS=""
GUI_LDLIBS=""
GUI_INCLUDES=""


# Determine C++ compiler - Parallel Build?
AC_ARG_WITH([parallel], [AC_HELP_STRING([--with-parallel], [Request parallel code (requires mpic++ in path) [default="no"]])], [with_parallel="yes"], [with_parallel="no"])
AM_CONDITIONAL([BUILDPARALLEL], [test "$with_parallel" = "yes"])
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


# Configuration options - Qt Tools
AC_ARG_WITH([qtmoc],[AC_HELP_STRING([--with-qtmoc=<executable>],
    [Specify Qt5 Meta-Object Compiler tool (moc) [default="moc-qt5"]])], ,
    [with_qtmoc="moc-qt5"])
AC_ARG_WITH([qtuic],[AC_HELP_STRING([--with-qtuic=<executable>],
    [Specify Qt5 User Interface Compiler tool (uic) [default="uic-qt5"]])], ,
    [with_qtuic="uic-qt5"])
AC_ARG_WITH([qtrcc],[AC_HELP_STRING([--with-qtrcc=<executable],
    [Specify Qt5 Resource Compiler tool (rcc) [default="rcc-qt5"]])], ,
    [with_qtrcc="rcc-qt5"])
QTMOC="$with_qtmoc"
QTRCC="$with_qtrcc"
QTUIC="$with_qtuic"

# Compile with internal object checking for debugging / testing?
AC_ARG_WITH([checks], [AC_HELP_STRING([--with-checks], [Enable internal code array/pointer checking [default="no"]])], [with_checks="yes"], [with_checks="no"])
if test "$with_checks" = "yes"; then
  CHECK_CFLAGS="-DCHECKS"
fi

# Compile GUI elements (which require Qt)
AC_ARG_WITH([gui], [AC_HELP_STRING([--with-gui], [Enable building of GUI [default="no"]])], [with_gui="yes"], [with_gui="no"])
AM_CONDITIONAL([BUILDGUI], [test "$with_gui" = "yes"])
if test "$with_gui" = "yes"; then
  # Check for required Qt packages
  PKG_CHECK_MODULES(QTGUI, Qt5Gui >= 5.0.0)
  PKG_CHECK_MODULES(QTOPENGL, Qt5OpenGL >= 5.0.0)
  PKG_CHECK_MODULES(QTWIDGETS, Qt5Widgets >= 5.0.0)
  PKG_CHECK_MODULES(QTPRINT, Qt5PrintSupport >= 5.0.0)

  # Make sure Yacc/Bison is available
  AC_PROG_YACC

  # Find Freetype and FTGL (with pkgconfig)
  PKG_CHECK_MODULES(FREETYPE, freetype2 >= 2.0)
  PKG_CHECK_MODULES(FTGL, ftgl >= 2.0)

  # Update CFLAGS, includes etc.
  GUI_LDLIBS="$QTGUI_LIBS $QTOPENGL_LIBS $QTPRINT_LIBS $FTGL_LIBS $FREETYPE_LIBS -lGL"
  GUI_CFLAGS="-fPIC"
  GUI_INCLUDES=" $QTGUI_CFLAGS $QTWIDGETS_CFLAGS $QTPRINT_CFLAGS $FTGL_CFLAGS"
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
VERSION=DISSOLVE_VERSION
AC_SUBST(VERSION)
DISSOLVE_CFLAGS="$PARALLEL_CFLAGS $CHECK_CFLAGS"
AC_SUBST(DISSOLVE_CFLAGS)
DISSOLVE_READLINE="-L$RLINCDIR"
AC_SUBST(DISSOLVE_READLINE)
AC_SUBST(QTMOC)
AC_SUBST(QTUIC)
AC_SUBST(QTRCC)
AC_SUBST(GUI_INCLUDES)
AC_SUBST(GUI_CFLAGS)
AC_SUBST(GUI_LDLIBS)


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
AC_CONFIG_FILES(src/analyse/Makefile)
AC_CONFIG_FILES(src/analyse/nodes/Makefile)
AC_CONFIG_FILES(src/base/Makefile)
AC_CONFIG_FILES(src/classes/Makefile)
AC_CONFIG_FILES(src/data/Makefile)
AC_CONFIG_FILES(src/main/Makefile)
AC_CONFIG_FILES(src/math/Makefile)
AC_CONFIG_FILES(src/module/Makefile)
AC_CONFIG_FILES(src/module/keywordtypes/Makefile)
AC_CONFIG_FILES(src/modules/Makefile)
AC_CONFIG_FILES(src/modules/analyse/Makefile)
AC_CONFIG_FILES(src/modules/atomshake/Makefile)
AC_CONFIG_FILES(src/modules/bragg/Makefile)
AC_CONFIG_FILES(src/modules/calculate/Makefile)
AC_CONFIG_FILES(src/modules/calculate/rdf/Makefile)
AC_CONFIG_FILES(src/modules/calculate/dangle/Makefile)
AC_CONFIG_FILES(src/modules/calibration/Makefile)
AC_CONFIG_FILES(src/modules/checks/Makefile)
AC_CONFIG_FILES(src/modules/datatest/Makefile)
AC_CONFIG_FILES(src/modules/energy/Makefile)
AC_CONFIG_FILES(src/modules/epsr/Makefile)
AC_CONFIG_FILES(src/modules/export/Makefile)
AC_CONFIG_FILES(src/modules/forces/Makefile)
AC_CONFIG_FILES(src/modules/import/Makefile)
AC_CONFIG_FILES(src/modules/intrashake/Makefile)
AC_CONFIG_FILES(src/modules/md/Makefile)
AC_CONFIG_FILES(src/modules/molshake/Makefile)
AC_CONFIG_FILES(src/modules/neutronsq/Makefile)
AC_CONFIG_FILES(src/modules/rdf/Makefile)
AC_CONFIG_FILES(src/modules/refine/Makefile)
AC_CONFIG_FILES(src/modules/sanitycheck/Makefile)
AC_CONFIG_FILES(src/modules/skeleton/Makefile)
AC_CONFIG_FILES(src/modules/sq/Makefile)
AC_CONFIG_FILES(src/modules/test/Makefile)
AC_CONFIG_FILES(src/templates/Makefile)
if test "$with_gui" = "yes"; then
  AC_CONFIG_FILES(src/gui/Makefile)
  AC_CONFIG_FILES(src/gui/delegates/Makefile)
  AC_CONFIG_FILES(src/gui/keywordwidgets/Makefile)
  AC_CONFIG_FILES(src/gui/uchroma/Makefile)
  AC_CONFIG_FILES(src/gui/widgets/Makefile)
  AC_CONFIG_FILES(src/gui/uchroma/classes/Makefile)
  AC_CONFIG_FILES(src/gui/uchroma/expression/Makefile)
  AC_CONFIG_FILES(src/gui/uchroma/gui/Makefile)
  AC_CONFIG_FILES(src/gui/uchroma/kernels/Makefile)
  AC_CONFIG_FILES(src/gui/uchroma/render/Makefile)
  AC_CONFIG_FILES(src/gui/uchroma/session/Makefile)
  AC_CONFIG_FILES(src/modules/analyse/gui/Makefile)
  AC_CONFIG_FILES(src/modules/atomshake/gui/Makefile)
  AC_CONFIG_FILES(src/modules/bragg/gui/Makefile)
  AC_CONFIG_FILES(src/modules/calculate/rdf/gui/Makefile)
  AC_CONFIG_FILES(src/modules/calculate/dangle/gui/Makefile)
  AC_CONFIG_FILES(src/modules/calibration/gui/Makefile)
  AC_CONFIG_FILES(src/modules/datatest/gui/Makefile)
  AC_CONFIG_FILES(src/modules/energy/gui/Makefile)
  AC_CONFIG_FILES(src/modules/epsr/gui/Makefile)
  AC_CONFIG_FILES(src/modules/neutronsq/gui/Makefile)
  AC_CONFIG_FILES(src/modules/rdf/gui/Makefile)
  AC_CONFIG_FILES(src/modules/refine/gui/Makefile)
  AC_CONFIG_FILES(src/modules/skeleton/gui/Makefile)
  AC_CONFIG_FILES(src/modules/sq/gui/Makefile)
  AC_CONFIG_FILES(src/modules/test/gui/Makefile)
fi
AC_OUTPUT


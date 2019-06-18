SUBDIRS = src

SUFFIXES: .dt

.dt.desktop:
	sed -e "s:PREFIX:@INSTALLDIR@:" -e "s:VERSION:@VERSION@:" $< > $@

# 'Fake' target to prevent autotools complaining about lack of README (==> README.md)
README: README.md

# 'Fake' target to prevent autotools complaining about lack of ChangeLog (==> ChangeLog.md)
ChangeLog: ChangeLog.md

EXTRA_DIST = CMakeLists.txt src/CMakeLists.txt src/*/CMakeLists.txt src/gui/*/CMakeLists.txt src/gui/uchroma/*/CMakeLists.txt
EXTRA_DIST += src/gui/icons/*.* src/gui/uchroma/gui/icons/* src/gui/uchroma/gui/freefont/*

#epsrdir = $(datadir)/epsr
#epsr_DATA = extra/epsr.png

#desktopdir = @BUILDDIR@/usr/share/applications
#desktop_DATA = Dissolve.desktop


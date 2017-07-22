SUBDIRS = src

SUFFIXES: .dt

.dt.desktop:
	sed -e "s:PREFIX:@INSTALLDIR@:" -e "s:VERSION:@VERSION@:" $< > $@

EXTRA_DIST = TODO CMakeLists.txt src/CMakeLists.txt src/*/CMakeLists.txt

#epsrdir = $(datadir)/epsr
#epsr_DATA = extra/epsr.png

#desktopdir = @BUILDDIR@/usr/share/applications
#desktop_DATA = dUQ.desktop


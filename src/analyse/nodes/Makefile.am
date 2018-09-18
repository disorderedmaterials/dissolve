noinst_LTLIBRARIES = libanalysisnodes.la

libanalysisnodes_la_SOURCES = calculate.cpp collect1d.cpp collect2d.cpp collect3d.cpp
libanalysisnodes_la_SOURCES += dynamicselect.cpp
libanalysisnodes_la_SOURCES += exclude.cpp
libanalysisnodes_la_SOURCES += node.cpp normalise1d.cpp normalise2d.cpp normalise3d.cpp
libanalysisnodes_la_SOURCES += select.cpp selectbase.cpp sequence.cpp


noinst_HEADERS = calculate.h
noinst_HEADERS += collect1d.h collect2d.h collect3d.h
noinst_HEADERS += dynamicselect.h
noinst_HEADERS += exclude.h
noinst_HEADERS += node.h nodes.h normalise1d.h normalise2d.h normalise3d.h
noinst_HEADERS += select.h selectbase.h sequence.h

AM_CPPFLAGS = -I../ -I../../ @DISSOLVE_CFLAGS@ @DISSOLVE_READLINE@


noinst_LTLIBRARIES = libneta.la

AM_YFLAGS = -d

clean-local:
	-rm -f neta_grammar.cc neta_grammar.hh

libneta_la_SOURCES = neta_grammar.yy
libneta_la_SOURCES += connection.cpp generator.cpp logic.cpp neta.cpp node.cpp presence.cpp ring.cpp root.cpp

noinst_HEADERS = connection.h generator.h logic.h neta.h node.h presence.h ring.h root.h

AM_CPPFLAGS = -I$(top_srcdir)/src -I../ -I./ @DISSOLVE_CFLAGS@ @GUI_CFLAGS@ @GUI_INCLUDES@


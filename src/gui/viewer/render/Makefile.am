noinst_LTLIBRARIES = librender.la

AM_YFLAGS = -d

clean-local:
	-rm -f textprimitive_grammar.cc textprimitive_grammar.hh

librender_la_SOURCES = textprimitive_grammar.yy

librender_la_SOURCES += fontinstance.cpp
librender_la_SOURCES += primitive.cpp primitiveinfo.cpp primitiveinstance.cpp #primitivevertex.cpp
librender_la_SOURCES += textprimitive.cpp
librender_la_SOURCES += vertexchunk.cpp

noinst_HEADERS = fontinstance.h primitive.h primitiveinfo.h primitiveinstance.h textprimitive.h vertexchunk.h

AM_CPPFLAGS = -I$(top_srcdir)/src -I../ -I./ @DISSOLVE_CFLAGS@ @GUI_CFLAGS@ @GUI_INCLUDES@


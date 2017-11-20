noinst_LTLIBRARIES = librender.la

AM_YFLAGS = -d

clean-local:
	-rm -f textprimitive_grammar.cc textprimitive_grammar.hh

librender_la_SOURCES = textprimitive_grammar.yy
librender_la_SOURCES += fontinstance.cpp linestipple.cpp linestyle.cpp primitive.cpp primitiveinfo.cpp primitiveinstance.cpp primitivelist.cpp surface.cpp surface_full.cpp surface_grid.cpp surface_linexy.cpp surface_linezy.cpp textformat.cpp textfragment.cpp textprimitive.cpp textprimitivelist.cpp

noinst_HEADERS = fontinstance.h linestipple.h linestyle.h primitive.h primitiveinfo.h primitiveinstance.h primitivelist.h surface.h textformat.h textfragment.h textprimitive.h textprimitivelist.h

AM_CPPFLAGS = -I$(top_srcdir)/src -I../ -I./ @DUQ_CFLAGS@ @GUI_CFLAGS@ @GUI_INCLUDES@


noinst_LTLIBRARIES = librender.la

AM_YFLAGS = -d

clean-local:
	-rm -f textprimitive_grammar.cc textprimitive_grammar.hh

librender_la_SOURCES = textprimitive_grammar.yy

librender_la_SOURCES += axes.cpp
librender_la_SOURCES += collection.cpp collectiongroup.cpp
librender_la_SOURCES += displaydataset.cpp
librender_la_SOURCES += fontinstance.cpp
librender_la_SOURCES += numberformat.cpp
librender_la_SOURCES += primitive.cpp primitiveinfo.cpp primitiveinstance.cpp #primitivevertex.cpp
librender_la_SOURCES += targetdata.cpp targetprimitive.cpp textprimitive.cpp
librender_la_SOURCES += vertexchunk.cpp view.cpp

noinst_HEADERS = axes.h
noinst_HEADERS += collection.h collectiongroup.h
noinst_HEADERS += displaydataset.h
noinst_HEADERS += fontinstance.h
noinst_HEADERS += numberformat.h
noinst_HEADERS += primitive.h primitiveinfo.h primitiveinstance.h
noinst_HEADERS += targetdata.h targetprimitive.h textprimitive.h
noinst_HEADERS += vertexchunk.h view.h

AM_CPPFLAGS = -I$(top_srcdir)/src -I../ -I./ @DISSOLVE_CFLAGS@ @GUI_CFLAGS@ @GUI_INCLUDES@


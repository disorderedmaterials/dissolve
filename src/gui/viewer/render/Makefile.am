noinst_LTLIBRARIES = librender.la

AM_YFLAGS = -d

clean-local:
	-rm -f textprimitive_grammar.cc textprimitive_grammar.hh

librender_la_SOURCES = textprimitive_grammar.yy

librender_la_SOURCES += axes.cpp
librender_la_SOURCES += colourdefinition.cpp colourscale.cpp colourscaledelta.cpp colourscalepoint.cpp
librender_la_SOURCES += fontinstance.cpp
librender_la_SOURCES += linestipple.cpp linestyle.cpp
librender_la_SOURCES += numberformat.cpp
librender_la_SOURCES += primitive.cpp primitiveinfo.cpp primitiveinstance.cpp primitivelist.cpp #primitivevertex.cpp
librender_la_SOURCES += renderable.cpp renderabledata1d.cpp renderablefactory.cpp renderablegroup.cpp renderablegroupmanager.cpp
librender_la_SOURCES += symbol.cpp
librender_la_SOURCES += textformat.cpp textfragment.cpp textprimitive.cpp textprimitivelist.cpp
librender_la_SOURCES += vertexchunk.cpp view.cpp

noinst_HEADERS = axes.h
noinst_HEADERS += colourdefinition.h colourscale.h colourscaledelta.h colourscalepoint.h
noinst_HEADERS += fontinstance.h
noinst_HEADERS += linestipple.cpp linestyle.cpp
noinst_HEADERS += numberformat.h
noinst_HEADERS += primitive.h primitiveinfo.h primitiveinstance.h primitivelist.h
noinst_HEADERS += renderable.h renderabledata1d.h renderablefactory.h renderablegroup.h renderablegroupmanager.h
noinst_HEADERS += symbol.h
noinst_HEADERS += textformat.h textfragment.h textprimitive.h textprimitivelist.h
noinst_HEADERS += vertexchunk.h view.h

AM_CPPFLAGS = -I$(top_srcdir)/src -I../ -I./ @DISSOLVE_CFLAGS@ @GUI_CFLAGS@ @GUI_INCLUDES@


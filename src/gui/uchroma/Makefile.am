noinst_LTLIBRARIES = libuchroma.la

SUBDIRS = gui classes expression kernels render session

libuchroma_la_SOURCES = uchromabase_collections.cpp uchromabase_funcs.cpp uchromabase_image.cpp uchromabase_interaction.cpp uchromabase_undoredo.cpp uchromabase_view.cpp
libuchroma_la_LIBADD = gui/libgui.la classes/libclasses.la expression/libexpression.la kernels/libkernels.la render/librender.la session/libsession.la 
libuchroma_la_LDFLAGS = -static

noinst_HEADERS = uchromabase.h

libuchroma_la_CPPFLAGS = -I$(top_srcdir)/src @DUQ_CFLAGS@ @GUI_CFLAGS@ @GUI_INCLUDES@



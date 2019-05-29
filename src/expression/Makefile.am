noinst_LTLIBRARIES = libexpression.la

AM_YFLAGS = -d

clean-local:
	-rm -f expression_grammar.cc expression_grammar.hh

libexpression_la_SOURCES = expression_grammar.yy
libexpression_la_SOURCES += expression.cpp expression_lexer.cpp function.cpp functions.cpp node.cpp value.cpp variable.cpp

noinst_HEADERS = expression.h function.h functions.h node.h value.h variable.h

AM_CPPFLAGS = -I$(top_srcdir)/src -I../ -I./ @DISSOLVE_CFLAGS@ @GUI_CFLAGS@ @GUI_INCLUDES@


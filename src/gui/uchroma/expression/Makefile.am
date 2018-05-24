noinst_LTLIBRARIES = libexpression.la

AM_YFLAGS = -d

clean-local:
	-rm -f expression_grammar.cc expression_grammar.hh

libexpression_la_SOURCES = expression_grammar.yy
libexpression_la_SOURCES += expression.cpp expression_lexer.cpp functionnode.cpp functions.cpp node.cpp variable.cpp variablenode.cpp

noinst_HEADERS = expression.h functionnode.h functions.h node.h variable.h variablenode.h

AM_CPPFLAGS = -I$(top_srcdir)/src -I../ -I./ @DISSOLVE_CFLAGS@ @GUI_CFLAGS@ @GUI_INCLUDES@


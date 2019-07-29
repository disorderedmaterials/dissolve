noinst_LTLIBRARIES = libexpression.la

AM_YFLAGS = -d

clean-local:
	-rm -f generator_grammar.cc generator_grammar.hh

libexpression_la_SOURCES = generator_grammar.yy
libexpression_la_SOURCES += expression.cpp function.cpp functions.cpp generator.cpp node.cpp value.cpp variable.cpp variablevalue.cpp

noinst_HEADERS = expression.h function.h functions.h generator.h node.h value.h variable.h variablevalue.h

AM_CPPFLAGS = -I$(top_srcdir)/src -I../ -I./ @DISSOLVE_CFLAGS@ @GUI_CFLAGS@ @GUI_INCLUDES@


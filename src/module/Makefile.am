noinst_LTLIBRARIES = libmodule.la

libmodule_la_SOURCES = keywordbase.cpp keywordlist.cpp list.cpp module.cpp reference.cpp registry.cpp
libmodule_la_LIBADD = 
libmodule_la_LDFLAGS = -static

noinst_HEADERS = keywordbase.h keyworddata.h keywordlist.h keywordtypes.h list.h module.h reference.h registry.h

AM_CPPFLAGS = -I../ -I../../ @DUQ_CFLAGS@ @DUQ_READLINE@

SUBDIRS = keywordtypes


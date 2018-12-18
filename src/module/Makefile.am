noinst_LTLIBRARIES = libmodule.la

libmodule_la_SOURCES = group.cpp keywordbase.cpp keywordlist.cpp list.cpp module.cpp reference.cpp
libmodule_la_LIBADD = 
libmodule_la_LDFLAGS = -static

noinst_HEADERS = group.h keywordbase.h keyworddata.h keywordlist.h keywordtypes.h list.h module.h reference.h registrar.h

AM_CPPFLAGS = -I../ -I../../ @DISSOLVE_CFLAGS@ @DISSOLVE_READLINE@

SUBDIRS = keywordtypes


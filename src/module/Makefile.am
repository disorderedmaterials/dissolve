noinst_LTLIBRARIES = libmodule.la

libmodule_la_SOURCES = group.cpp groups.cpp keywordbase.cpp keywordgroup.cpp keywordlist.cpp layer.cpp list.cpp module.cpp
libmodule_la_LIBADD = 
libmodule_la_LDFLAGS = -static

noinst_HEADERS = group.h groups.h keywordbase.h keyworddata.h keywordgroup.h keywordlist.h keywordtypes.h layer.h list.h module.h

AM_CPPFLAGS = -I../ -I../../ @DISSOLVE_CFLAGS@ @DISSOLVE_READLINE@

SUBDIRS = keywordtypes


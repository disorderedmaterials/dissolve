noinst_LTLIBRARIES = libmain.la

libmain_la_SOURCES = atomtypes.cpp comms.cpp configurations.cpp dissolve.cpp io.cpp layers.cpp masterterms.cpp modules.cpp pairpotentials.cpp setup.cpp simulation.cpp species.cpp

libmain_la_SOURCES += keywords.cpp keywords_configuration.cpp keywords_layer.cpp keywords_master.cpp keywords_module.cpp keywords_pairpotentials.cpp keywords_simulation.cpp

noinst_HEADERS = dissolve.h keywords.h

AM_CPPFLAGS = -I../ -I../../ @DISSOLVE_CFLAGS@

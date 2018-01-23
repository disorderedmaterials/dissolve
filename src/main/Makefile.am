noinst_LTLIBRARIES = libmain.la

libmain_la_SOURCES = atomtypes.cpp comms.cpp configurations.cpp data.cpp duq.cpp io.cpp masterterms.cpp pairpotentials.cpp setup.cpp simulation.cpp species.cpp

libmain_la_SOURCES += keywords.cpp keywords_configuration.cpp keywords_data.cpp keywords_master.cpp keywords_module.cpp keywords_pairpotentials.cpp keywords_simulation.cpp keywords_species.cpp keywords_speciesinfo.cpp

noinst_HEADERS = duq.h keywords.h

AM_CPPFLAGS = -I../ -I../../ @DUQ_CFLAGS@ @DUQ_READLINE@

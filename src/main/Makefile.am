noinst_LIBRARIES = libmain.a

libmain_a_SOURCES = atomtypes.cpp comms.cpp duq.cpp io_configuration.cpp io_input.cpp modules.cpp pairpotentials.cpp samples.cpp simulation.cpp species.cpp

libmain_a_SOURCES += keywords.cpp keywords_atomtypes.cpp keywords_configuration.cpp keywords_module.cpp keywords_pairpotentials.cpp keywords_sample.cpp keywords_simulation.cpp keywords_species.cpp

libmain_a_SOURCES += methods_energy.cpp methods_forces.cpp methods_grainshake.cpp methods_intrashake.cpp methods_md.cpp methods_perturb.cpp methods_twist.cpp

noinst_HEADERS = duq.h keywords.h

AM_CPPFLAGS = -I../ -I../../ @DUQ_CFLAGS@ @DUQ_READLINE@

noinst_LIBRARIES = libmain.a

libmain_a_SOURCES = atomtypes.cpp command.cpp commands.cpp commands_basic.cpp commands_test.cpp comms.cpp composition.cpp duq.cpp flags.cpp io_configuration.cpp io_input.cpp keywords.cpp pairpotentials.cpp partials.cpp partials_bragg.cpp partials_intra.cpp partials_simple.cpp samples.cpp setup.cpp simulation.cpp species.cpp

libmain_a_SOURCES += methods_atomshake.cpp methods_energy.cpp methods_forces.cpp methods_grainshake.cpp methods_intrashake.cpp methods_md.cpp methods_perturb.cpp methods_twist.cpp methods_upkeep.cpp

noinst_HEADERS = command.h duq.h flags.h keywords.h

AM_CPPFLAGS = -I../ -I../../ @DUQ_CFLAGS@ @DUQ_READLINE@

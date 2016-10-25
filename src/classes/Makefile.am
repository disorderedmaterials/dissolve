noinst_LIBRARIES = libclasses.a

libclasses_a_SOURCES = atom.cpp atomtype.cpp atomtypedata.cpp atomtypeindex.cpp
libclasses_a_SOURCES += box.cpp box_cubic.cpp box_monoclinic.cpp box_nonperiodic.cpp box_orthorhombic.cpp box_triclinic.cpp braggpeak.cpp
libclasses_a_SOURCES += cell.cpp cellneighbour.cpp changedata.cpp changestore.cpp configuration.cpp configuration_box.cpp configuration_composition.cpp configuration_contents.cpp configuration_data.cpp configuration_energy.cpp configuration_partials.cpp configuration_upkeep.cpp
libclasses_a_SOURCES += energykernel.cpp
libclasses_a_SOURCES += forcekernel.cpp
libclasses_a_SOURCES += grain.cpp
libclasses_a_SOURCES += histogram.cpp
libclasses_a_SOURCES += isotopologue.cpp isotopologuemix.cpp
libclasses_a_SOURCES += kvector.cpp
libclasses_a_SOURCES += molecule.cpp
libclasses_a_SOURCES += pairpotential.cpp potentialmap.cpp
libclasses_a_SOURCES += sample.cpp species.cpp species_atomic.cpp species_grains.cpp species_intra.cpp species_io.cpp species_isotopologues.cpp species_transform.cpp speciesangle.cpp speciesatom.cpp speciesbond.cpp speciesgrain.cpp species_io.cpp

noinst_HEADERS = atom.h atomtype.h atomtypedata.h atomtypeindex.h box.h braggpeak.h cell.h cellneighbour.h changedata.h changestore.h configuration.h energykernel.h forcekernel.h grain.h histogram.h isotopologue.h isotopologuemix.h kvector.h molecule.h pairpotential.h potentialmap.h sample.h species.h speciesangle.h speciesatom.h speciesbond.h speciesgrain.h

AM_CPPFLAGS = -I../ @DUQ_CFLAGS@

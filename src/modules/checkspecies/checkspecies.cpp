// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "modules/checkspecies/checkspecies.h"
#include "keywords/double.h"
#include "keywords/species.h"
#include "keywords/vector_intdouble.h"
#include "keywords/vector_intstring.h"

CheckSpeciesModule::CheckSpeciesModule() : Module("CheckSpecies")
{
    // Target Species
    keywords_.addTarget<SpeciesKeyword>("Species", "Target species to check", targetSpecies_);

    // Checks
    keywords_.add<IntegerStringVectorKeyword>("Atoms", "AtomType", "Check that atom index <id> has the atom type <type>",
                                              atomTypes_, 1, 1);
    keywords_.add<DoubleKeyword>("Atoms", "ChargeTolerance", "Tolerance beyond which charges are said to differ",
                                 chargeTolerance_, 1.0e-5);
    keywords_.add<DoubleKeyword>("Atoms", "TotalCharge", "Check the total charge of the species against the specified value",
                                 totalCharge_);
    keywords_.add<IntegerDoubleVectorKeyword>("Intramolecular", "Bond",
                                              "Check that the bond <i>-<j> has the correct parameters", bondParameters_, 2);
    keywords_.add<IntegerDoubleVectorKeyword>(
        "Intramolecular", "Angle", "Check that the angle <i>-<j>-<k> has the correct parameters", angleParameters_, 3);
    keywords_.add<IntegerDoubleVectorKeyword>("Intramolecular", "Torsion",
                                              "Check that the torsion <i>-<j>-<k>-<l> has the correct parameters",
                                              torsionParameters_, 4);
    keywords_.add<IntegerDoubleVectorKeyword>("Intramolecular", "Improper",
                                              "Check that the improper <i>-<j>-<k>-<l> has the correct parameters",
                                              improperParameters_, 4);
    keywords_.add<DoubleKeyword>("Intramolecular", "Tolerance", "Tolerance beyond which parameters are said to differ",
                                 tolerance_, 1.0e-5);
}

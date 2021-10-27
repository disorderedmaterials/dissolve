// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "keywords/types.h"
#include "modules/checkspecies/checkspecies.h"

// Perform any necessary initialisation for the Module
void CheckSpeciesModule::initialise()
{
    // Target Species
    keywords_.add<SpeciesKeyword>("Target", "Species", "Target species to check", targetSpecies_);

    // Checks
    keywords_.add("Atoms", new IntegerStringVectorKeyword(1, 1), "AtomType",
                  "Check that atom index <id> has the atom type <type>");
    keywords_.add<DoubleKeyword>("Atoms", "ChargeTolerance", "Tolerance beyond which charges are said to differ",
                                 chargeTolerance_, 1.0e-5);
    keywords_.add<DoubleKeyword>("Atoms", "TotalCharge", "Check the total charge of the species against the specified value",
                                 totalCharge_);
    keywords_.add("Intramolecular", new IntegerDoubleVectorKeyword(2), "Bond",
                  "Check that the bond <i>-<j> has the correct parameters");
    keywords_.add("Intramolecular", new IntegerDoubleVectorKeyword(3), "Angle",
                  "Check that the angle <i>-<j>-<k> has the correct parameters");
    keywords_.add("Intramolecular", new IntegerDoubleVectorKeyword(4), "Torsion",
                  "Check that the torsion <i>-<j>-<k>-<l> has the correct parameters");
    keywords_.add("Intramolecular", new IntegerDoubleVectorKeyword(4), "Improper",
                  "Check that the improper <i>-<j>-<k>-<l> has the correct parameters");
    keywords_.add<DoubleKeyword>("Intramolecular", "Tolerance", "Tolerance beyond which parameters are said to differ",
                                 tolerance_, 1.0e-5);
}

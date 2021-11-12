// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "keywords/types.h"
#include "modules/checkspecies/checkspecies.h"

// Perform any necessary initialisation for the Module
void CheckSpeciesModule::initialise()
{
    // Target Species
    keywords_.add("Target", new SpeciesKeyword(), "Species", "Target species to check");

    // Checks
    keywords_.add("Atoms", new IntegerStringVectorKeyword(1, 1), "AtomType",
                  "Check that atom index <id> has the atom type <type>");
    keywords_.add("Atoms", new DoubleKeyword(1.0e-3), "ChargeTolerance", "Tolerance beyond which charges are said to differ");
    keywords_.add("Atoms", new DoubleKeyword(0.0), "TotalCharge",
                  "Check the total charge of the species against the specified value");
    keywords_.add("Intramolecular", new IntegerDoubleVectorKeyword(2), "Bond",
                  "Check that the bond <i>-<j> has the correct parameters");
    keywords_.add("Intramolecular", new IntegerDoubleVectorKeyword(3), "Angle",
                  "Check that the angle <i>-<j>-<k> has the correct parameters");
    keywords_.add("Intramolecular", new IntegerDoubleVectorKeyword(4), "Torsion",
                  "Check that the torsion <i>-<j>-<k>-<l> has the correct parameters");
    keywords_.add("Intramolecular", new IntegerDoubleVectorKeyword(4), "Improper",
                  "Check that the improper <i>-<j>-<k>-<l> has the correct parameters");
    keywords_.add("Intramolecular", new DoubleKeyword(1.0e-3), "Tolerance",
                  "Tolerance beyond which parameters are said to differ");
}

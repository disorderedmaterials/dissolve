// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "keywords/types.h"
#include "modules/intrashake/intrashake.h"

// Perform any necessary initialisation for the Module
void IntraShakeModule::initialise()
{
    // Control
    keywords_.add("Control", new DoubleKeyword(-1.0), "CutoffDistance", "Interatomic cutoff distance to employ", "<distance>");
    keywords_.add("Control", new IntegerKeyword(1), "ShakesPerTerm", "Number of shakes per term", "<n>");
    keywords_.add("Control", new DoubleKeyword(0.33), "TargetAcceptanceRate", "Target acceptance rate for Monte Carlo moves",
                  "<rate (0.0-1.0)>");
    keywords_.add("Control", new BoolKeyword(false), "TermEnergyOnly",
                  "Whether only the energy of the intramolecular term is calculated and assessed");

    // Bonds
    keywords_.add("Bonds", new BoolKeyword(true), "AdjustBonds", "Whether Bonds in the molecule should be shaken",
                  "<true|false>");
    keywords_.add("Bonds", new DoubleKeyword(0.01), "BondStepSize", "Step size for Bond adjustments (Angstroms)", "<stepsize>",
                  KeywordBase::InRestartFileOption);
    keywords_.add("Bonds", new DoubleKeyword(0.001), "BondStepSizeMin", "Minimum step size for Bond adjustments (Angstroms)",
                  "<stepsize>");
    keywords_.add("Bonds", new DoubleKeyword(0.2), "BondStepSizeMax", "Maximum step size for Bond adjustments (Angstroms)",
                  "<stepsize>");

    // Angles
    keywords_.add("Angles", new BoolKeyword(true), "AdjustAngles", "Whether Angles in the molecule should be shaken",
                  "<true|false>");
    keywords_.add("Angles", new DoubleKeyword(5.0), "AngleStepSize", "Step size for Angle adjustments (degrees)", "<stepsize>",
                  KeywordBase::InRestartFileOption);
    keywords_.add("Angles", new DoubleKeyword(0.01), "AngleStepSizeMin", "Minimum step size for Angle adjustments (degrees)",
                  "<stepsize>");
    keywords_.add("Angles", new DoubleKeyword(20.0), "AngleStepSizeMax", "Maximum step size for Angle adjustments (degrees)",
                  "<stepsize>");

    // Torsions
    keywords_.add("Torsions", new BoolKeyword(true), "AdjustTorsions", "Whether Torsions in the molecule should be shaken",
                  "<true|false>");
    keywords_.add("Torsions", new DoubleKeyword(10.0), "TorsionStepSize", "Step size for Torsion adjustments (degrees)",
                  "<stepsize>", KeywordBase::InRestartFileOption);
    keywords_.add("Torsions", new DoubleKeyword(0.5), "TorsionStepSizeMin",
                  "Minimum step size for Torsion adjustments (degrees)", "<stepsize>");
    keywords_.add("Torsions", new DoubleKeyword(45.0), "TorsionStepSizeMax",
                  "Maximum step size for Torsion adjustments (degrees)", "<stepsize>");
}

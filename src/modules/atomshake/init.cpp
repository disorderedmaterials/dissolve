// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#include "keywords/types.h"
#include "modules/atomshake/atomshake.h"

// Perform any necessary initialisation for the Module
void AtomShakeModule::initialise()
{
    // Calculation
    keywords_.add("Calculation", new DoubleKeyword(-1.0, -1.0), "CutoffDistance", "Interatomic cutoff distance to employ");
    keywords_.add("Calculation", new IntegerKeyword(1, 1, 1000), "ShakesPerAtom", "Number of shakes per Atom to attempt");
    keywords_.add("Calculation", new DoubleKeyword(0.33, 0.01, 1.0), "TargetAcceptanceRate",
                  "Target acceptance rate for Monte Carlo moves");

    // Translations
    keywords_.add("Translations", new DoubleKeyword(0.05), "StepSize",
                  "Step size for translational component of Monte Carlo move (Angstroms)", "<stepsize>",
                  KeywordBase::InRestartFileOption);
    keywords_.add("Translations", new DoubleKeyword(1.0), "StepSizeMax", "Maximum step size for translations (Angstroms)",
                  "<stepsize>");
    keywords_.add("Translations", new DoubleKeyword(0.001), "StepSizeMin", "Minimum step size for translations (Angstroms)",
                  "<stepsize>");
}

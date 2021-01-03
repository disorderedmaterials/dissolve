// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "keywords/types.h"
#include "modules/atomshake/atomshake.h"

// Perform any necessary initialisation for the Module
void AtomShakeModule::initialise()
{
    // Control
    keywords_.add("Control", new DoubleKeyword(-1.0, -1.0), "CutoffDistance",
                  "Interatomic cutoff distance to use for energy calculation");
    keywords_.add("Control", new IntegerKeyword(1, 1, 1000), "ShakesPerAtom", "Number of shakes to attempt per atom");
    keywords_.add("Control", new DoubleKeyword(0.05), "StepSize", "Step size in Angstroms to use in Monte Carlo moves",
                  "<stepsize>", KeywordBase::InRestartFileOption);
    keywords_.add("Control", new DoubleKeyword(1.0), "StepSizeMax", "Maximum step size for translations (Angstroms)",
                  "<stepsize>");
    keywords_.add("Control", new DoubleKeyword(0.001), "StepSizeMin", "Minimum allowed value for step size, in Angstroms",
                  "<stepsize>");
    keywords_.add("Control", new DoubleKeyword(0.33, 0.01, 1.0), "TargetAcceptanceRate",
                  "Target acceptance rate for Monte Carlo moves");
}

// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#include "keywords/types.h"
#include "modules/molshake/molshake.h"

// Perform any necessary initialisation for the Module
void MolShakeModule::initialise()
{
    // Control
    keywords_.add("Control", new DoubleKeyword(-1.0), "CutoffDistance", "Interatomic cutoff distance to employ", "<rcut>");
    keywords_.add("Control", new IntegerKeyword(1), "ShakesPerMolecule", "Number of shakes per Molecule", "<n>");
    keywords_.add("Control", new DoubleKeyword(0.33), "TargetAcceptanceRate",
                  "Target acceptance rate for Monte Carlo moves", "<rate (0.0 - 1.0)>");

    // Rotations
    keywords_.add("Rotations", new DoubleKeyword(1.0), "RotationStepSize",
                  "Step size for rotational component of Monte Carlo move (degrees)", "<stepsize>",
                  KeywordBase::InRestartFileOption);
    keywords_.add("Rotations", new DoubleKeyword(0.01), "RotationStepSizeMin", "Minimum step size for rotations (degrees)",
                  "<stepsize>");
    keywords_.add("Rotations", new DoubleKeyword(90.0), "RotationStepSizeMax", "Maximum step size for rotations (degrees)",
                  "<stepsize>");

    // Translations
    keywords_.add("Translations", new DoubleKeyword(0.05), "TranslationStepSize",
                  "Step size for translational component of Monte Carlo move (Angstroms)", "<stepsize>",
                  KeywordBase::InRestartFileOption);
    keywords_.add("Translations", new DoubleKeyword(0.001), "TranslationStepSizeMin",
                  "Minimum step size for translations (Angstroms)", "<stepsize>");
    keywords_.add("Translations", new DoubleKeyword(3.0), "TranslationStepSizeMax",
                  "Maximum step size for translations (Angstroms)", "<stepsize>");
}

// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "keywords/double.h"
#include "keywords/integer.h"
#include "keywords/speciesvector.h"
#include "modules/molshake/molshake.h"

// Perform any necessary initialisation for the Module
void MolShakeModule::initialise()
{
    // Control
    keywords_.add<DoubleKeyword>("Control", "CutoffDistance", "Interatomic cutoff distance to use for energy calculation",
                                 cutoffDistance_, -1.0);
    keywords_.add<IntegerKeyword>("Control", "ShakesPerMolecule", "Number of shakes to attempt per molecule",
                                  nShakesPerMolecule_, 1);
    keywords_.add<DoubleKeyword>("Control", "TargetAcceptanceRate", "Target acceptance rate for Monte Carlo moves",
                                 targetAcceptanceRate_, 0.001);
    keywords_
        .add<DoubleKeyword>("Control", "RotationStepSize",
                            "Step size in degrees to use for the rotational component of the Monte Carlo moves",
                            rotationStepSize_)
        .optionMask = KeywordBase::InRestartFileOption;
    keywords_.add<DoubleKeyword>("Control", "RotationStepSizeMin", "Minimum step size for rotations (degrees)",
                                 rotationStepSizeMin_, 0.001, 180.0);
    keywords_.add<DoubleKeyword>("Control", "RotationStepSizeMax", "Maximum step size for rotations (degrees)",
                                 rotationStepSizeMax_, 0.001, 180.0);
    keywords_
        .add<DoubleKeyword>("Control", "TranslationStepSize",
                            "Step size in Angstroms for the translational component of the Monte Carlo moves",
                            translationStepSize_, 0.0001, 100.0)
        .optionMask = KeywordBase::InRestartFileOption;
    keywords_.add<DoubleKeyword>("Control", "TranslationStepSizeMin", "Minimum step size for translations (Angstroms)",
                                 translationStepSizeMin_, 0.0001);
    keywords_.add<DoubleKeyword>("Control", "TranslationStepSizeMax", "Maximum step size for translations (Angstroms)",
                                 translationStepSizeMax_, 100.0);
    keywords_.add("Control", new SpeciesVectorKeyword(), "RestrictToSpecies",
                  "Restrict the calculation to the specified Species");
}

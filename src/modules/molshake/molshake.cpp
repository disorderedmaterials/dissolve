// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "modules/molshake/molshake.h"
#include "keywords/bool.h"
#include "keywords/configuration.h"
#include "keywords/double.h"
#include "keywords/integer.h"
#include "keywords/optionaldouble.h"
#include "keywords/speciesvector.h"

MolShakeModule::MolShakeModule() : Module("MolShake")
{
    // Targets
    keywords_.addTarget<ConfigurationKeyword>("Configuration", "Set target configuration for the module", targetConfiguration_);

    // Control
    keywords_.add<OptionalDoubleKeyword>(
        "Control", "CutoffDistance",
        "Interatomic cutoff distance to use for energy calculation (0.0 to use pair potential range)", cutoffDistance_, 0.0,
        std::nullopt, 0.1, "Use PairPotential Range");
    keywords_.add<IntegerKeyword>("Control", "ShakesPerMolecule", "Number of shakes to attempt per molecule",
                                  nShakesPerMolecule_, 1);
    keywords_.add<DoubleKeyword>("Control", "TargetAcceptanceRate", "Target acceptance rate for Monte Carlo moves",
                                 targetAcceptanceRate_, 0.001);
    keywords_.addRestartable<DoubleKeyword>("Control", "RotationStepSize",
                                            "Step size in degrees to use for the rotational component of the Monte Carlo moves",
                                            rotationStepSize_);
    keywords_.add<DoubleKeyword>("Control", "RotationStepSizeMin", "Minimum step size for rotations (degrees)",
                                 rotationStepSizeMin_, 0.001, 180.0);
    keywords_.add<DoubleKeyword>("Control", "RotationStepSizeMax", "Maximum step size for rotations (degrees)",
                                 rotationStepSizeMax_, 0.001, 180.0);
    keywords_.addRestartable<DoubleKeyword>("Control", "TranslationStepSize",
                                            "Step size in Angstroms for the translational component of the Monte Carlo moves",
                                            translationStepSize_, 0.0001, 100.0);
    keywords_.add<DoubleKeyword>("Control", "TranslationStepSizeMin", "Minimum step size for translations (Angstroms)",
                                 translationStepSizeMin_, 0.0001);
    keywords_.add<DoubleKeyword>("Control", "TranslationStepSizeMax", "Maximum step size for translations (Angstroms)",
                                 translationStepSizeMax_, 100.0);
    keywords_.add<SpeciesVectorKeyword>("Control", "RestrictToSpecies", "Restrict the calculation to the specified Species",
                                        restrictToSpecies_);
}

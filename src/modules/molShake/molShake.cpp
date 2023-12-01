// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "modules/molShake/molShake.h"
#include "keywords/bool.h"
#include "keywords/configuration.h"
#include "keywords/double.h"
#include "keywords/integer.h"
#include "keywords/optionalDouble.h"
#include "keywords/speciesVector.h"

MolShakeModule::MolShakeModule() : Module(ModuleTypes::MolShake)
{
    keywords_.addTarget<ConfigurationKeyword>("Configuration", "Set target configuration for the module", targetConfiguration_);

    keywords_.setOrganisation("Options", "Control", "Number of move attempts per atom and the target acceptance rate.");
    keywords_.add<IntegerKeyword>("ShakesPerMolecule", "Number of shakes to attempt per molecule", nShakesPerMolecule_, 1);
    keywords_.add<DoubleKeyword>("TargetAcceptanceRate", "Target acceptance rate for Monte Carlo moves", targetAcceptanceRate_,
                                 0.001);
    keywords_.add<SpeciesVectorKeyword>("RestrictToSpecies", "Restrict the calculation to the specified Species",
                                        restrictToSpecies_);

    keywords_.setOrganisation("Options", "Rotations",
                              "Current step size and limits for Monte Carlo rotations. The step size is dynamically updated as "
                              "the calculation proceeds in order to approximate the specified acceptance rate.");
    keywords_.addRestartable<DoubleKeyword>("RotationStepSize",
                                            "Step size in degrees to use for the rotational component of the Monte Carlo moves",
                                            rotationStepSize_);
    keywords_.add<DoubleKeyword>("RotationStepSizeMin", "Minimum step size for rotations (degrees)", rotationStepSizeMin_,
                                 0.001, 180.0);
    keywords_.add<DoubleKeyword>("RotationStepSizeMax", "Maximum step size for rotations (degrees)", rotationStepSizeMax_,
                                 0.001, 180.0);

    keywords_.setOrganisation(
        "Options", "Translations",
        "Current step size and limits for Monte Carlo translations. The step size is dynamically updated as "
        "the calculation proceeds in order to approximate the specified acceptance rate.");
    keywords_.addRestartable<DoubleKeyword>("TranslationStepSize",
                                            "Step size in Angstroms for the translational component of the Monte Carlo moves",
                                            translationStepSize_, 0.0001, 100.0);
    keywords_.add<DoubleKeyword>("TranslationStepSizeMin", "Minimum step size for translations (Angstroms)",
                                 translationStepSizeMin_, 0.0001, 100.0);
    keywords_.add<DoubleKeyword>("TranslationStepSizeMax", "Maximum step size for translations (Angstroms)",
                                 translationStepSizeMax_, 0.0001, 100.0);

    keywords_.setOrganisation("Advanced");
    keywords_.add<OptionalDoubleKeyword>(
        "CutoffDistance", "Interatomic cutoff distance to use for energy calculation (0.0 to use pair potential range)",
        cutoffDistance_, 0.0, std::nullopt, 0.1, "Use PairPotential Range");
}

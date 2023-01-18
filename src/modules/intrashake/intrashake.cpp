// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "modules/intrashake/intrashake.h"
#include "keywords/bool.h"
#include "keywords/configuration.h"
#include "keywords/double.h"
#include "keywords/integer.h"
#include "keywords/optionaldouble.h"
#include "keywords/speciesvector.h"

IntraShakeModule::IntraShakeModule() : Module("IntraShake")
{
    // Targets
    keywords_.addTarget<ConfigurationKeyword>("Configuration", "Set target configuration for the module", targetConfiguration_);

    // Control
    keywords_.add<OptionalDoubleKeyword>(
        "Control", "CutoffDistance",
        "Interatomic cutoff distance to use for energy calculation (0.0 to use pair potential range)", cutoffDistance_, 0.0,
        std::nullopt, 0.1, "Use PairPotential Range");
    keywords_.add<IntegerKeyword>("Control", "ShakesPerTerm", "Number of shakes per term", nShakesPerTerm_, 1);
    keywords_.add<DoubleKeyword>("Control", "TargetAcceptanceRate", "Target acceptance rate for Monte Carlo moves",
                                 targetAcceptanceRate_, 0.001, 1.0);
    keywords_.add<BoolKeyword>("Control", "TermEnergyOnly",
                               "Whether only the energy of the intramolecular term is calculated and assessed",
                               termEnergyOnly_);
    keywords_.add<SpeciesVectorKeyword>("Control", "RestrictToSpecies", "Restrict the calculation to the specified Species",
                                        restrictToSpecies_);

    // Bonds
    keywords_.add<BoolKeyword>("Control", "AdjustBonds", "Whether bonds in molecules should be shaken", adjustBonds_);
    keywords_.addRestartable<DoubleKeyword>("Control", "BondStepSize", "Step size for bond adjustments (Angstroms)",
                                            bondStepSize_, 0.001, 1.0);
    keywords_.add<DoubleKeyword>("Control", "BondStepSizeMin", "Minimum step size for bond adjustments (Angstroms)",
                                 bondStepSizeMin_, 0.001, 1.0);
    keywords_.add<DoubleKeyword>("Control", "BondStepSizeMax", "Maximum step size for bond adjustments (Angstroms)",
                                 bondStepSizeMax_, 0.001, 1.0);

    // Angles
    keywords_.add<BoolKeyword>("Control", "AdjustAngles", "Whether angles in molecules should be shaken", adjustAngles_);
    keywords_.addRestartable<DoubleKeyword>("Control", "AngleStepSize", "Step size for angle adjustments (degrees)",
                                            angleStepSize_, 0.01, 45.0);
    keywords_.add<DoubleKeyword>("Control", "AngleStepSizeMin", "Minimum step size for angle adjustments (degrees)",
                                 angleStepSizeMin_, 0.01, 45.0);
    keywords_.add<DoubleKeyword>("Control", "AngleStepSizeMax", "Maximum step size for angle adjustments (degrees)",
                                 angleStepSizeMax_, 0.01, 45.0);

    // Torsions
    keywords_.add<BoolKeyword>("Control", "AdjustTorsions", "Whether torsions in molecules should be shaken", adjustTorsions_);
    keywords_.addRestartable<DoubleKeyword>("Control", "TorsionStepSize", "Step size for torsion adjustments (degrees)",
                                            torsionStepSize_, 0.01, 45.0);
    keywords_.add<DoubleKeyword>("Control", "TorsionStepSizeMin", "Minimum step size for torsion adjustments (degrees)",
                                 torsionStepSizeMin_, 0.01, 45.0);
    keywords_.add<DoubleKeyword>("Control", "TorsionStepSizeMax", "Maximum step size for torsion adjustments (degrees)",
                                 torsionStepSizeMax_, 0.01, 45.0);
}

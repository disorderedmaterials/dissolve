// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "modules/intrashake/intrashake.h"
#include "keywords/bool.h"
#include "keywords/double.h"
#include "keywords/integer.h"
#include "keywords/optionaldouble.h"

IntraShakeModule::IntraShakeModule() : Module("IntraShake")
{
    // Targets
    keywords_.addTarget<ConfigurationVectorKeyword>("Configuration", "Set target configuration(s) for the module",
                                                    targetConfigurations_);

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

    // Bonds
    keywords_.add<BoolKeyword>("Bonds", "AdjustBonds", "Whether Bonds in the molecule should be shaken", adjustBonds_);
    keywords_.addRestartable<DoubleKeyword>("Bonds", "BondStepSize", "Step size for Bond adjustments (Angstroms)",
                                            bondStepSize_, 0.001, 1.0);
    keywords_.add<DoubleKeyword>("Bonds", "BondStepSizeMin", "Minimum step size for Bond adjustments (Angstroms)",
                                 bondStepSizeMin_, 0.001, 1.0);
    keywords_.add<DoubleKeyword>("Bonds", "BondStepSizeMax", "Maximum step size for Bond adjustments (Angstroms)",
                                 bondStepSizeMax_, 0.001, 1.0);

    // Angles
    keywords_.add<BoolKeyword>("Angles", "AdjustAngles", "Whether Angles in the molecule should be shaken", adjustAngles_);
    keywords_.addRestartable<DoubleKeyword>("Angles", "AngleStepSize", "Step size for Angle adjustments (degrees)",
                                            angleStepSize_, 0.01, 45.0);
    keywords_.add<DoubleKeyword>("Angles", "AngleStepSizeMin", "Minimum step size for Angle adjustments (degrees)",
                                 angleStepSizeMin_, 0.01, 45.0);
    keywords_.add<DoubleKeyword>("Angles", "AngleStepSizeMax", "Maximum step size for Angle adjustments (degrees)",
                                 angleStepSizeMax_, 0.01, 45.0);

    // Torsions
    keywords_.add<BoolKeyword>("Torsions", "AdjustTorsions", "Whether Torsions in the molecule should be shaken",
                               adjustTorsions_);
    keywords_.addRestartable<DoubleKeyword>("Torsions", "TorsionStepSize", "Step size for Torsion adjustments (degrees)",
                                            torsionStepSize_, 0.01, 45.0);
    keywords_.add<DoubleKeyword>("Torsions", "TorsionStepSizeMin", "Minimum step size for Torsion adjustments (degrees)",
                                 torsionStepSizeMin_, 0.01, 45.0);
    keywords_.add<DoubleKeyword>("Torsions", "TorsionStepSizeMax", "Maximum step size for Torsion adjustments (degrees)",
                                 torsionStepSizeMax_, 0.01, 45.0);
}

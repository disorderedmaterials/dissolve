// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "modules/intraShake/intraShake.h"
#include "keywords/bool.h"
#include "keywords/configuration.h"
#include "keywords/double.h"
#include "keywords/integer.h"
#include "keywords/optionalDouble.h"
#include "keywords/speciesVector.h"

IntraShakeModule::IntraShakeModule() : Module(ModuleTypes::IntraShake)
{
    keywords_.addTarget<ConfigurationKeyword>("Configuration", "Set target configuration for the module", targetConfiguration_);

    keywords_.setOrganisation("Options", "Control", "Number of move attempts per term and the target acceptance rate.");
    keywords_.add<IntegerKeyword>("ShakesPerTerm", "Number of shakes per term", nShakesPerTerm_, 1);
    keywords_.add<DoubleKeyword>("TargetAcceptanceRate", "Target acceptance rate for Monte Carlo moves", targetAcceptanceRate_,
                                 0.001, 1.0);
    keywords_.add<BoolKeyword>(
        "TermEnergyOnly", "Whether only the energy of the intramolecular term is calculated and assessed", termEnergyOnly_);
    keywords_.add<SpeciesVectorKeyword>("RestrictToSpecies", "Restrict the calculation to the specified Species",
                                        restrictToSpecies_);

    keywords_.setOrganisation(
        "Options", "Bonds",
        "Current step size and limits for Monte Carlo moves on bonds. The step size is dynamically updated as "
        "the calculation proceeds in order to approximate the specified acceptance rate.");
    keywords_.add<BoolKeyword>("AdjustBonds", "Whether bonds in molecules should be shaken", adjustBonds_);
    keywords_.addRestartable<DoubleKeyword>("BondStepSize", "Step size for bond adjustments (Angstroms)", bondStepSize_, 0.001,
                                            1.0);
    keywords_.add<DoubleKeyword>("BondStepSizeMin", "Minimum step size for bond adjustments (Angstroms)", bondStepSizeMin_,
                                 0.001, 1.0);
    keywords_.add<DoubleKeyword>("BondStepSizeMax", "Maximum step size for bond adjustments (Angstroms)", bondStepSizeMax_,
                                 0.001, 1.0);

    keywords_.setOrganisation(
        "Options", "Angles",
        "Current step size and limits for Monte Carlo moves on angles. The step size is dynamically updated as "
        "the calculation proceeds in order to approximate the specified acceptance rate.");
    keywords_.add<BoolKeyword>("AdjustAngles", "Whether angles in molecules should be shaken", adjustAngles_);
    keywords_.addRestartable<DoubleKeyword>("AngleStepSize", "Step size for angle adjustments (degrees)", angleStepSize_, 0.01,
                                            45.0);
    keywords_.add<DoubleKeyword>("AngleStepSizeMin", "Minimum step size for angle adjustments (degrees)", angleStepSizeMin_,
                                 0.01, 45.0);
    keywords_.add<DoubleKeyword>("AngleStepSizeMax", "Maximum step size for angle adjustments (degrees)", angleStepSizeMax_,
                                 0.01, 45.0);

    keywords_.setOrganisation(
        "Options", "Torsions",
        "Current step size and limits for Monte Carlo moves on torsions. The step size is dynamically updated as "
        "the calculation proceeds in order to approximate the specified acceptance rate.");
    keywords_.add<BoolKeyword>("AdjustTorsions", "Whether torsions in molecules should be shaken", adjustTorsions_);
    keywords_.addRestartable<DoubleKeyword>("TorsionStepSize", "Step size for torsion adjustments (degrees)", torsionStepSize_,
                                            0.01, 45.0);
    keywords_.add<DoubleKeyword>("TorsionStepSizeMin", "Minimum step size for torsion adjustments (degrees)",
                                 torsionStepSizeMin_, 0.01, 45.0);
    keywords_.add<DoubleKeyword>("TorsionStepSizeMax", "Maximum step size for torsion adjustments (degrees)",
                                 torsionStepSizeMax_, 0.01, 45.0);

    keywords_.setOrganisation("Advanced");
    keywords_.add<OptionalDoubleKeyword>(
        "CutoffDistance", "Interatomic cutoff distance to use for energy calculation (0.0 to use pair potential range)",
        cutoffDistance_, 0.0, std::nullopt, 0.1, "Use PairPotential Range");
}

// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "keywords/types.h"
#include "modules/atomshake/atomshake.h"

// Perform any necessary initialisation for the Module
void AtomShakeModule::initialise()
{
    // Targets
    keywords_.addTarget<ConfigurationVectorKeyword>("Configuration", "Set target configuration(s) for the module",
                                                    targetConfigurations_);

    // Control
    keywords_.add<DoubleKeyword>("Control", "CutoffDistance", "Interatomic cutoff distance to use for energy calculation",
                                 cutoffDistance_, -1.0);
    keywords_.add<IntegerKeyword>("Control", "ShakesPerAtom", "Number of shakes to attempt per atom", nShakesPerAtom_, 1);
    keywords_.add<DoubleKeyword>("Control", "StepSize", "Step size in Angstroms to use in Monte Carlo moves", stepSize_, 0.001)
        ->setOptionMask(KeywordBase::InRestartFileOption);
    keywords_.add<DoubleKeyword>("Control", "StepSizeMax", "Maximum allowed value for step size, in Angstroms", stepSizeMax_,
                                 0.01);
    keywords_.add<DoubleKeyword>("Control", "StepSizeMin", "Minimum allowed value for step size, in Angstroms", stepSizeMin_,
                                 0.001);
    keywords_.add<DoubleKeyword>("Control", "TargetAcceptanceRate", "Target acceptance rate for Monte Carlo moves",
                                 targetAcceptanceRate_, 0.01, 1.0);
}

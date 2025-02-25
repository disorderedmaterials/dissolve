// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "nodes/atomShake.h"

AtomShakeNode::AtomShakeNode()
{
    addInput<Configuration *>("Configuration", "Set target configuration for the module", targetConfiguration_, nullptr);
    //        ->setEditSignals({KeywordBase::ClearModuleData, KeywordBase::RecreateRenderables});

    //    keywords_.setOrganisation("Options", "Control", "Number of move attempts per atom and the target acceptance rate.");
    addInput<int>("ShakesPerAtom", "Number of shakes to attempt per atom", nShakesPerAtom_, 1);
    addBoundedInput<double>("TargetAcceptanceRate", "Target acceptance rate for Monte Carlo moves", targetAcceptanceRate_, 0.01,
                            0, 1);

    //    keywords_.setOrganisation("Options", "Step Size",
    //                              "Current step size and limits for the Monte Carlo move. The step size is dynamically updated
    //                              as " "the calculation proceeds in order to approximate the specified acceptance rate.");
    //    keywords_.addRestartable<DoubleKeyword>("StepSize", "Step size in Angstroms to use in Monte Carlo moves", stepSize_,
    //    0.001);
    addInput<double>("StepSizeMax", "Maximum allowed value for step size, in Angstroms", stepSizeMax_, 0.01);
    addInput<double>("StepSizeMin", "Minimum allowed value for step size, in Angstroms", stepSizeMin_, 1.0e-4);

    // THIS KEYWORD HAS NEVER BEEN USED AFAIK, SO I THINK IT (AND OTHERS) CAN BE REMOVED AS WE MOVE FORWARD
    //    keywords_.setOrganisation("Advanced");
    //    keywords_.add<OptionalDoubleKeyword>(
    //        "CutoffDistance", "Interatomic cutoff distance to use for energy calculation (0.0 to use pair potential range)",
    //        cutoffDistance_, 0.0, std::nullopt, 0.1, "Use PairPotential Range");

    // executeIfTargetsUnchanged_ = true;
}

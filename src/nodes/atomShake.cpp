// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "nodes/atomShake.h"

AtomShakeNode::AtomShakeNode(Graph *parentGraph) : Node(parentGraph)
{
    addInput<Configuration *>("Configuration", "Set target configuration for the module", targetConfiguration_)
        ->setFlags(ParameterBase::ClearData);

    //    keywords_.setOrganisation("Options", "Control", "Number of move attempts per atom and the target acceptance rate.");
    addInput<int>("ShakesPerAtom", "Number of shakes to attempt per atom", nShakesPerAtom_);
    addBoundedInput<double>("TargetAcceptanceRate", "Target acceptance rate for Monte Carlo moves", targetAcceptanceRate_, 0,
                            1);

    //    keywords_.setOrganisation("Options", "Step Size",
    //                              "Current step size and limits for the Monte Carlo move. The step size is dynamically updated
    //                              as " "the calculation proceeds in order to approximate the specified acceptance rate.");
    //    keywords_.addRestartable<DoubleKeyword>("StepSize", "Step size in Angstroms to use in Monte Carlo moves", stepSize_,
    //    0.001);
    addInput<double>("StepSizeMax", "Maximum allowed value for step size, in Angstroms", stepSizeMax_);
    addInput<double>("StepSizeMin", "Minimum allowed value for step size, in Angstroms", stepSizeMin_);

    // THIS KEYWORD HAS NEVER BEEN USED AFAIK, SO I THINK IT (AND OTHERS) CAN BE REMOVED AS WE MOVE FORWARD
    //    keywords_.setOrganisation("Advanced");
    addBoundedOptionalInput<std::optional<double>>(
        "CuttOffDistance", "Interatomic cutoff distance to use for energy calculation (0.0 to use pair potential range)",
        cutoffDistance_, 0.0, "Use PairPotential Range", {}, 0.1);

    // executeIfTargetsUnchanged_ = true;
}

std::string_view AtomShakeNode::type() const { return "Atom Shake"; }

std::string_view AtomShakeNode::summary() const
{
    return "Perform an atomic Monte Carlo \"shake\" of every atom in each target configuration.";
}

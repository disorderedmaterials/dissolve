// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "nodes/atomShake/atomShake.h"

AtomShakeNode::AtomShakeNode(Graph *parentGraph) : Node(parentGraph)
{
    addInput<Configuration *>("Configuration", "Set target configuration for the module", targetConfiguration_)
        ->setFlags({ParameterBase::Required, ParameterBase::ClearData});

    addOption<Number>("ShakesPerAtom", "Number of shakes to attempt per atom", nShakesPerAtom_);
    addOption<Number>("TargetAcceptanceRate", "Target acceptance rate for Monte Carlo moves", targetAcceptanceRate_);
    addOption<Number>("StepSizeMax", "Maximum allowed value for step size, in Angstroms", stepSizeMax_);
    addOption<Number>("StepSizeMin", "Minimum allowed value for step size, in Angstroms", stepSizeMin_);
}

std::string_view AtomShakeNode::type() const { return "AtomShake"; }

std::string_view AtomShakeNode::summary() const
{
    return "Perform a Monte Carlo trial move on every atom in the target configuration.";
}

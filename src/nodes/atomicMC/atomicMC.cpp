// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "nodes/atomicMC/atomicMC.h"

AtomicMCNode::AtomicMCNode(Graph *parentGraph) : Node(parentGraph)
{
    // Inputs
    addInput<Configuration *>("Configuration", "Set target configuration for the module", targetConfiguration_)
        ->setFlags({ParameterBase::Required, ParameterBase::ClearData});

    // Options
    addOption<Number>("ShakesPerAtom", "Number of shakes to attempt per atom", nShakesPerAtom_);
    addOption<Number>("TargetAcceptanceRate", "Target acceptance rate for Monte Carlo moves", targetAcceptanceRate_);
    addOption<Number>("StepSizeMax", "Maximum allowed value for step size, in Angstroms", stepSizeMax_);
    addOption<Number>("StepSizeMin", "Minimum allowed value for step size, in Angstroms", stepSizeMin_);

    // Outputs
    addOutput<Configuration *>("Configuration", "Output configuration", targetConfiguration_);

    // Serialisables
    addSerialisable("stepSize", stepSize_);
}

std::string_view AtomicMCNode::type() const { return "AtomicMC"; }

std::string_view AtomicMCNode::summary() const
{
    return "Perform a Monte Carlo trial move on every atom in the target configuration.";
}

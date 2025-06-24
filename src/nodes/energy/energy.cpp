// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "nodes/energy/energy.h"

EnergyNode::EnergyNode(Graph *parentGraph) : Node(parentGraph)
{
    addInput<Configuration *>("Configuration", "Set target configuration for the module", targetConfiguration_)
        ->setFlags({ParameterBase::Required, ParameterBase::ClearData});

    addOption<double>("StabilityThreshold",
                      "Threshold value at which energy is deemed stable over the defined windowing period",
                      stabilityThreshold_);
    addOption<int>("StabilityWindow", "Number of points over which to assess the stability of the energy (per Configuration)",
                   stabilityWindow_);

    addOption<bool>("Save", "Save calculated energies to disk, one file per targeted configuration", save_);
}

std::string_view EnergyNode::type() const { return "Energy"; }

std::string_view EnergyNode::summary() const { return "Calculate total system energy and its contributions"; }

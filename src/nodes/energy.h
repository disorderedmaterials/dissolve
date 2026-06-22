// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "math/history.h"
#include "nodes/node.h"

class EnergyNode : public Node
{
    public:
    EnergyNode(Graph *parentGraph);
    ~EnergyNode() override = default;

    /*
     * Definition
     */
    public:
    std::string_view type() const override;
    std::string_view summary() const override;

    /*
     * Data
     */
    private:
    // Target configuration
    Configuration *targetConfiguration_{nullptr};
    // Threshold value at which energy is deemed stable over the defined windowing period
    double stabilityThreshold_{0.001};
    // Number of points over which to assess the stability of the energy (per Configuration)
    int stabilityWindow_{10};
    // Energy histories
    PODHistory<double> totalEnergyHistory_;
    PODHistory<double> totalPairPotentialHistory_, totalMoleculePPHistory_, totalGeometryHistory_, totalCohesiveHistory;
    PODHistory<double> bondHistory_, angleHistory_, torsionHistory_, improperHistory_;

    public:
    // Energy Stability Enum
    enum EnergyStability
    {
        NotAssessable = -1,
        EnergyStable = 0,
        EnergyUnstable = 1
    };

    /*
     * Processing
     */
    private:
    // Perform processing
    NodeConstants::ProcessResult process() override;
};

// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "kernels/energy.h"
#include "math/history.h"
#include "nodes/node.h"

// Forward Declarations
class PotentialMap;

// Energy Node
class EnergyNode : public Node
{
    public:
    EnergyNode(Graph *parentGraph);
    ~EnergyNode() override = default;

    /*
     * Node
     */
    public:
    std::string_view type() const override;
    std::string_view summary() const override;

    private:
    // Run main processing
    NodeConstants::ProcessResult process() override;

    /*
     * Definition
     */
    private:
    // Target configuration
    Configuration *targetConfiguration_{nullptr};
    // Threshold value at which energy is deemed stable over the defined windowing period
    double stabilityThreshold_{0.001};
    // Number of points over which to assess the stability of the energy (per Configuration)
    int stabilityWindow_{10};

    /*
     * Calculated Data
     */
    private:
    // Energy histories
    PODHistory<double> totalEnergyHistory_;
    PODHistory<double> totalPairPotentialHistory_, totalMoleculePPHistory_, totalGeometryHistory_, totalCohesiveHistory;
    PODHistory<double> bondHistory_, angleHistory_, torsionHistory_, improperHistory_;

    /*
     * Functions
     */
    public:
    // Energy Stability Enum
    enum EnergyStability
    {
        NotAssessable = -1,
        EnergyStable = 0,
        EnergyUnstable = 1
    };
    // Calculate energy components
    static Kernel::EnergyResult
    calculateEnergy(const Configuration *cfg, const std::unique_ptr<EnergyKernel> &kernel,
                    Flags<Kernel::CalculationFlags> flags = {});
    // Calculate energy components with simple double-loops for testing
    static Kernel::EnergyResult
    calculateTestEnergy(const Configuration *cfg, const std::unique_ptr<EnergyKernel> &kernel);
};

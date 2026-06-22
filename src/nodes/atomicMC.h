// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "classes/configuration.h"
#include "nodes/node.h"
#include "nodes/number.h"

class AtomicMCNode : public Node
{
    public:
    AtomicMCNode(Graph *parentGraph);
    ~AtomicMCNode() override = default;

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
    // Target configurations
    Configuration *targetConfiguration_{nullptr};
    // Temperature (K)
    Number temperature_{300};
    // Interatomic cutoff distance to use for energy calculation
    std::optional<double> cutoffDistance_;
    // Number of shakes to attempt per atom
    Number nShakesPerAtom_{1};
    // Step size in Angstroms to use in Monte Carlo moves
    Number stepSize_{0.0001};
    // Maximum allowed value for step size, in Angstroms
    Number stepSizeMax_{1.0};
    // Minimum allowed value for step size, in Angstroms
    Number stepSizeMin_{0.0001};
    // Target acceptance rate for Monte Carlo moves
    Number targetAcceptanceRate_{0.33};

    /*
     * Processing
     */
    protected:
    // Perform processing
    NodeConstants::ProcessResult process() override;
};

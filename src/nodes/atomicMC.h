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
    // Return type of the node
    std::string_view type() const override;
    // Return short summary of the node's purpose
    std::string_view summary() const override;

    /*
     * Data
     */
    private:
    // Target configurations
    Configuration *targetConfiguration_{nullptr};
    // Temperature (K)
    Number temperature_{300, 0.0};
    // Number of shakes to attempt per atom
    Number nShakesPerAtom_{1, 1};
    // Step size in Angstroms to use in Monte Carlo moves
    double stepSize_{0.0001};
    // Maximum allowed value for step size, in Angstroms
    Number stepSizeMax_{1.0, 0.1, 10.0};
    // Minimum allowed value for step size, in Angstroms
    Number stepSizeMin_{0.001, 0.0001, 1.0};
    // Target acceptance rate for Monte Carlo moves
    Number targetAcceptanceRate_{0.33, 0.01, 1.0};

    /*
     * Processing
     */
    protected:
    // Perform processing
    NodeConstants::ProcessResult process() override;
};

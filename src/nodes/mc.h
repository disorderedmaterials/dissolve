// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "nodes/node.h"

// Forward Declarations
class Configuration;

class MCNode : public Node
{
    public:
    MCNode(Graph *parentGraph);
    ~MCNode() override = default;

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
    Number temperature_{300};
    // Number of shakes to attempt per molecule
    int nShakesPerMolecule_{1};
    // Step size in degrees to use for the rotational component of the Monte Carlo moves
    double rotationStepSize_{1.0};
    // Minimum step size for rotations (degrees)
    Number rotationStepSizeMin_{1.0, 0.01, 45.0};
    // Maximum step size for rotations (degrees)
    Number rotationStepSizeMax_{90.0, 1.0, 180.0};
    // Step size in Angstroms for the translational component of the Monte Carlo moves
    double translationStepSize_{0.05};
    // Minimum step size for translations (Angstroms)
    Number translationStepSizeMin_{0.001, 0.001, 1.0};
    // Maximum step size for translations (Angstroms)
    Number translationStepSizeMax_{3.0, 0.1, 10.0};
    // Target acceptance rate for Monte Carlo moves
    Number targetAcceptanceRate_{0.33, 0.01, 1.0};

    /*
     * Processing
     */
    protected:
    // Perform processing
    NodeConstants::ProcessResult process() override;
};

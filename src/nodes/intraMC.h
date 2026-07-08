// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "nodes/node.h"

// Forward Declarations
class Configuration;

class IntraMCNode : public Node
{
    public:
    IntraMCNode(Graph *parentGraph);
    ~IntraMCNode() override = default;

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
    // Number of attempts to make per move
    Number nTrials_{1, 1};
    // Whether to adjust bonds
    bool adjustBonds_{true};
    // Step size in Angstroms for bond moves
    double bondStepSize_{0.05};
    // Minimum step size for bonds (Angstroms)
    Number bondStepSizeMin_{0.001, 0.001, 1.0};
    // Maximum step size for bonds (Angstroms)
    Number bondStepSizeMax_{3.0, 0.1, 10.0};
    // Whether to adjust angles
    bool adjustAngles_{true};
    // Step size in degrees to use for angle moves
    double angleStepSize_{1.0};
    // Minimum step size for angles (degrees)
    Number angleStepSizeMin_{0.5, 0.01, 45.0};
    // Maximum step size for angles (degrees)
    Number angleStepSizeMax_{5.0, 1.0, 180.0};
    // Whether to adjust torsions
    bool adjustTorsions_{true};
    // Step size in degrees to use for torsion moves
    double torsionStepSize_{1.0};
    // Minimum step size for torsions (degrees)
    Number torsionStepSizeMin_{1.0, 0.01, 45.0};
    // Maximum step size for torsions (degrees)
    Number torsionStepSizeMax_{20.0, 1.0, 180.0};
    // Target acceptance rate for Monte Carlo moves
    Number targetAcceptanceRate_{0.33, 0.01, 1.0};

    /*
     * Processing
     */
    protected:
    // Perform processing
    NodeConstants::ProcessResult process() override;
};

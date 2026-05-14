// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "classes/structure.h"
#include "nodes/node.h"

// CalculateBonding Node
class CalculateBondingNode : public Node
{
    public:
    CalculateBondingNode(Graph *parentGraph);
    ~CalculateBondingNode() override = default;

    public:
    std::string_view type() const override;
    std::string_view summary() const override;

    /*
     * Definition
     */
    private:
    // Structure for which bonding will be recalculated
    Structure structure_;
    // Bonding tolerance
    Number bondingTolerance_{1.1};
    // Whether to prevent metallic bonding
    bool preventMetallicBonds_{true};
    // Whether to clear bonds before recalculating
    bool clear_{false};

    /*
     * Processing
     */
    private:
    // Run main processing
    NodeConstants::ProcessResult process() override;
};

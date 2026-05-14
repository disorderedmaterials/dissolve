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
    // Input structure
    Structure inputStructure_;
    // Output structure
    Structure outputStructure_;
    // Bonding tolerance
    Number tolerance_{1.1};
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

    public:
    static void calculate(Structure &structure, double tolerance = 1.1, bool clearBefore = true, bool preventMetallic = false);
};

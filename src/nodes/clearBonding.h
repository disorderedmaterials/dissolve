// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "classes/structure.h"
#include "nodes/node.h"

// ClearBonding Node
class ClearBondingNode : public Node
{
    public:
    ClearBondingNode(Graph *parentGraph);
    ~ClearBondingNode() override = default;

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

    /*
     * Processing
     */
    private:
    // Run main processing
    NodeConstants::ProcessResult process() override;
};

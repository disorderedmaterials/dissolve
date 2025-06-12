// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "classes/isotopologueSet.h"
#include "nodes/node.h"

class IsotopologueSetNode : public Node
{
    public:
    IsotopologueSetNode(Graph *parentGraph);
    ~IsotopologueSetNode() override = default;

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
    // Atomic species
    const Species *species_;
    // Isotopologues to use in weighting
    IsotopologueSet isotopologueSet_;

    /*
     * Processing
     */
    private:
    // Run main processing
    NodeConstants::ProcessResult process() override;
};
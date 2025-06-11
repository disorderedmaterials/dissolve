// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "classes/isotopologueSet.h"
#include "classes/isotopologue.h"
#include "nodes/node.h"

class IsotopologueNode : public Node
{
    public:
    IsotopologueNode(Graph *parentGraph);
    ~IsotopologueNode() override = default;

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
    // Isotopologues to use in weighting
    IsotopologueSet isotopologueSet_;
    //
    Isotopologue iso_{"Ar"};
    //
    double relativeWeight_{ 36 };

    /*
     * Processing
     */
    private:
    // Run main processing
    NodeConstants::ProcessResult process() override;
};
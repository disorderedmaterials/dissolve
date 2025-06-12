// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "classes/isotopologueWeight.h"
#include "classes/isotopologue.h"
#include "nodes/node.h"


class IsotopologueNode : public Node
{
public:
    IsotopologueNode(Graph* parentGraph);
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
    // Temporary defaults for Ar36
    Isotopologue isotopologue_;
    IsotopologueWeight isotopologueWeight_;
    std::string name_{"Ar"};
    double relativeWeight_{ 36 };

    /*
     * Processing
     */
private:
    // Run main processing
    NodeConstants::ProcessResult process() override;
};
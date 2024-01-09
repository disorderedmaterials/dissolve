// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "neta/node.h"

// Forward Declarations
class NETADefinition;

// NETA HydrogenCount Node
class NETAHydrogenCountNode : public NETANode
{
    public:
    NETAHydrogenCountNode(NETADefinition *parent, NETANode::ComparisonOperator op = NETANode::ComparisonOperator::EqualTo,
                          std::optional<int> value = std::nullopt);
    ~NETAHydrogenCountNode() override = default;

    /*
     * Bond Count
     */
    private:
    // Number of hydrogens value
    std::optional<int> value_;
    // Number of hydrogens value comparison operator
    NETANode::ComparisonOperator operator_;

    public:
    // Set hydrogen count requirements
    void set(ComparisonOperator op, int value);

    /*
     * Scoring
     */
    public:
    // Evaluate the node and return its score
    int score(const SpeciesAtom *i, NETAMatchedGroup &matchPath) const override;
};

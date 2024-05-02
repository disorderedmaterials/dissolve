// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "neta/node.h"

// Forward Declarations
class NETADefinition;

// NETA BondCount Node
class NETABondCountNode : public NETANode
{
    public:
    explicit NETABondCountNode(NETADefinition *parent, NETANode::ComparisonOperator op = NETANode::ComparisonOperator::EqualTo,
                               std::optional<int> value = std::nullopt);
    ~NETABondCountNode() override = default;

    /*
     * Bond Count
     */
    private:
    // Number of bonds value
    std::optional<int> value_;
    // Number of bonds value comparison operator
    NETANode::ComparisonOperator operator_;

    public:
    // Set bond count requirements
    void set(ComparisonOperator op, int value);

    /*
     * Scoring
     */
    public:
    // Evaluate the node and return its score
    int score(const SpeciesAtom *i, NETAMatchedGroup &matchPath) const override;
};

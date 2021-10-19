// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "neta/node.h"

// Forward Declarations
class NETADefinition;

// NETA BondCount Node
class NETABondCountNode : public NETANode
{
    public:
    NETABondCountNode(NETADefinition *parent);
    ~NETABondCountNode() override = default;

    /*
     * Bond Count
     */
    private:
    // Number of bonds value
    int value_;
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
    int score(const SpeciesAtom *i, std::vector<const SpeciesAtom *> &matchPath) const override;
};

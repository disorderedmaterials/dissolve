// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#pragma once

#include "neta/node.h"

// Forward Declarations
class NETADefinition;

// NETA HydrogenCount Node
class NETAHydrogenCountNode : public NETANode
{
    public:
    NETAHydrogenCountNode(NETADefinition *parent);
    ~NETAHydrogenCountNode() override = default;

    /*
     * Bond Count
     */
    private:
    // Number of hydrogens value
    int value_;
    // Number of hydrogens value comparison operator
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

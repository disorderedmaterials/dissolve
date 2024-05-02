// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "neta/node.h"

// Forward Declarations
class NETADefinition;

// NETA Or Node
class NETAOrNode : public NETANode
{
    public:
    // Node logic types
    enum class LogicType
    {
        And,
        Or,
        AndNot
    };
    NETAOrNode(NETADefinition *parent);
    ~NETAOrNode() override = default;

    /*
     * Node Sequence
     */
    private:
    // Node sequence
    NETASequence altNodes_;

    public:
    // Set alternative node sequence
    void setAlternativeNodes(NETASequence nodes);

    /*
     * Scoring
     */
    public:
    // Evaluate the node and return its score
    int score(const SpeciesAtom *i, NETAMatchedGroup &matchPath) const override;
};

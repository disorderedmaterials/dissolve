// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

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
    ~NETAOrNode() = default;

    /*
     * Scoring
     */
    public:
    // Evaluate the node and return its score
    int score(const SpeciesAtom *i, std::vector<const SpeciesAtom *> &matchPath) const override;
};

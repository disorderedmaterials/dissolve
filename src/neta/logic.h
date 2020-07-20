// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#pragma once

#include "neta/node.h"

// Forward Declarations
class NETADefinition;

// NETA Logic Node
class NETALogicNode : public NETANode
{
    public:
    // Node logic types
    enum LogicType
    {
        AndLogic,
        OrLogic,
        AndNotLogic
    };
    NETALogicNode(NETADefinition *parent, LogicType logic, NETANode *arg1, NETANode *arg2);
    ~NETALogicNode();

    private:
    // Logic type
    LogicType logic_;
    // Node targets
    NETANode *argument1_, *argument2_;

    /*
     * Scoring
     */
    public:
    // Evaluate the node and return its score
    int score(const SpeciesAtom *i, std::vector<const SpeciesAtom*> &matchPath) const;
};

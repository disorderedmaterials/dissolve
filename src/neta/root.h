// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "classes/speciesatom.h"
#include "neta/node.h"
#include <vector>

// Forward Declarations
class Element;
class ForcefieldAtomType;
class NETADefinition;

// NETA Root Node
class NETARootNode : public NETANode
{
    public:
    NETARootNode(NETADefinition *parent);
    ~NETARootNode() override = default;

    /*
     * Scoring
     */
    public:
    // Evaluate the node and return its score
    int score(const SpeciesAtom *i, std::vector<const SpeciesAtom *> &matchPath) const override;
};

// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#pragma once

#include "classes/speciesbond.h"
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
    ~NETARootNode();

    /*
     * Modifiers
     */
    private:
    // Number of bonds value
    int nBondsValue_;
    // Numbe of bonds value comparison operator
    NETANode::ComparisonOperator nBondsValueOperator_;
    // Number of hydrogens value
    int nHydrogensValue_;
    // Numbe of hydrogens value comparison operator
    NETANode::ComparisonOperator nHydrogensValueOperator_;

    public:
    // Available modifiers
    enum NETARootModifier
    {
        NBondsModifier,     /* 'nbonds' - Specifies number of bonds (default = -1) */
        NHydrogensModifier, /* 'nh' - Specifies number of hydrogens (default = -1) */
        nRootModifiers
    };
    // Return enum options for NETARootModifiers
    static EnumOptions<NETARootNode::NETARootModifier> modifiers();
    // Return whether the specified modifier is valid for this node
    bool isValidModifier(std::string_view s) const;
    // Set value and comparator for specified modifier
    bool setModifier(std::string_view modifier, ComparisonOperator op, int value);

    /*
     * Scoring
     */
    public:
    // Evaluate the node and return its score
    int score(const SpeciesAtom *i, std::vector<const SpeciesAtom *> &matchPath) const;
};

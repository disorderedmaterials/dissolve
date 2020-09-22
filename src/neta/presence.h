// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#pragma once

#include "classes/speciesbond.h"
#include "neta/node.h"
#include <memory>
#include <vector>

// Forward Declarations
class Element;
class ForcefieldAtomType;
class NETADefinition;

// NETA Character Node
class NETAPresenceNode : public NETANode
{
    public:
    NETAPresenceNode(NETADefinition *parent, std::vector<Element *> targetElements,
                     std::vector<std::reference_wrapper<const ForcefieldAtomType>> targetAtomTypes,
                     SpeciesBond::BondType bt = SpeciesBond::nBondTypes);
    ~NETAPresenceNode();

    private:
    // Array of elements that the current context atom may be
    std::vector<Element *> allowedElements_;
    // Array of ForcefieldAtomTypes that the current context atom may be
    std::vector<std::reference_wrapper<const ForcefieldAtomType>> allowedAtomTypes_;

    /*
     * Modifiers
     */
    private:
    // Repeat count value
    int repeatCount_;
    // Repeat count comparison operator
    NETANode::ComparisonOperator repeatCountOperator_;
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
    enum NETACharacterModifier
    {
        NBondsModifier,          /* 'nbonds' - Specifies number of bonds (default = -1) */
        NHydrogensModifier,      /* 'nh' - Specifies number of hydrogens (default = -1) */
        RepeatCharacterModifier, /* 'n' - Specifies the number of matches required (default = 1) */
        nCharacterModifiers
    };
    // Return enum options for NETACharacterModifiers
    static EnumOptions<NETAPresenceNode::NETACharacterModifier> modifiers();
    // Return whether the specified modifier is valid for this node
    bool isValidModifier(std::string_view s) const;
    // Set value and comparator for specified modifier
    bool setModifier(std::string_view modifier, ComparisonOperator op, int value);

    /*
     * Scoring
     */
    public:
    // Evaluate the node and return its score
    int score(const SpeciesAtom *i, RefList<const SpeciesAtom> &availableAtoms) const;
};

// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "classes/speciesbond.h"
#include "neta/node.h"
#include <memory>
#include <vector>

// Forward Declarations
class Element;
class ForcefieldAtomType;
class NETADefinition;

// NETA Presence Node
class NETAPresenceNode : public NETANode
{
    public:
    NETAPresenceNode(NETADefinition *parent, std::vector<Elements::Element> targetElements,
                     std::vector<std::reference_wrapper<const ForcefieldAtomType>> targetAtomTypes);
    ~NETAPresenceNode() = default;

    /*
     * Atom Targets
     */
    private:
    // Array of elements that the current context atom may be
    std::vector<Elements::Element> allowedElements_;
    // Array of ForcefieldAtomTypes that the current context atom may be
    std::vector<std::reference_wrapper<const ForcefieldAtomType>> allowedAtomTypes_;

    public:
    // Add element target to node
    bool addElementTarget(Elements::Element Z) override;
    // Add forcefield type target to node
    bool addFFTypeTarget(const ForcefieldAtomType &ffType) override;

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
    // Number of bonds value comparison operator
    NETANode::ComparisonOperator nBondsValueOperator_;
    // Number of hydrogens value
    int nHydrogensValue_;
    // Number of hydrogens value comparison operator
    NETANode::ComparisonOperator nHydrogensValueOperator_;

    public:
    // Available modifiers
    enum class NETACharacterModifier
    {
        NBonds,     /* 'nbonds' - Specifies number of bonds (default = -1) */
        NHydrogens, /* 'nh' - Specifies number of hydrogens (default = -1) */
        Repeat      /* 'n' - Specifies the number of matches required (default = 1) */
    };
    // Return enum options for NETACharacterModifiers
    static EnumOptions<NETAPresenceNode::NETACharacterModifier> modifiers();
    // Return whether the specified modifier is valid for this node
    bool isValidModifier(std::string_view s) const override;
    // Set value and comparator for specified modifier
    bool setModifier(std::string_view modifier, ComparisonOperator op, int value) override;

    /*
     * Scoring
     */
    public:
    // Evaluate the node and return its score
    int score(const SpeciesAtom *i, std::vector<const SpeciesAtom *> &availableAtoms) const;
};

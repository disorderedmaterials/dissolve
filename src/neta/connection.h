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

// NETA Connection Node
class NETAConnectionNode : public NETANode
{
    public:
    NETAConnectionNode(NETADefinition *parent, std::vector<std::reference_wrapper<const Elements::Element>> targetElements = {},
                       std::vector<std::reference_wrapper<const ForcefieldAtomType>> targetAtomTypes = {},
                       SpeciesBond::BondType bt = SpeciesBond::nBondTypes);
    ~NETAConnectionNode();

    /*
     * Atom Targets
     */
    private:
    // Array of elements that the current context atom may be
    std::vector<std::reference_wrapper<const Elements::Element>> allowedElements_;
    // Array of ForcefieldAtomTypes that the current context atom may be
    std::vector<std::reference_wrapper<const ForcefieldAtomType>> allowedAtomTypes_;
    // Type of required connection
    SpeciesBond::BondType bondType_;

    public:
    // Add element target to node
    bool addElementTarget(const Elements::Element &el);
    // Add forcefield type target to node
    bool addFFTypeTarget(const ForcefieldAtomType &ffType);

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
    enum NETAConnectionModifier
    {
        NBondsModifier,           /* 'nbonds' - Specifies number of bonds (default = -1) */
        NHydrogensModifier,       /* 'nh' - Specifies number of hydrogens (default = -1) */
        RepeatConnectionModifier, /* 'n' - Specifies the number of matches required (default = 1) */
        nConnectionModifiers
    };
    // Return enum options for NETAConnectionModifiers
    static EnumOptions<NETAConnectionNode::NETAConnectionModifier> modifiers();
    // Return whether the specified modifier is valid for this node
    bool isValidModifier(std::string_view s) const;
    // Set value and comparator for specified modifier
    bool setModifier(std::string_view modifier, ComparisonOperator op, int value);

    /*
     * Flags
     */
    private:
    // Whether to allow the root atom of the path to be re-matched within this node
    bool allowRootMatch_;

    public:
    // Available flags
    enum NETAConnectionFlag
    {
        RootFlag, /* 'root' - Specifies that the root atom of the current path may be re-matched */
        nConnectionFlags
    };
    // Return enum options for NETAConnectionFlags
    static EnumOptions<NETAConnectionNode::NETAConnectionFlag> flags();
    // Return whether the specified flag is valid for this node
    bool isValidFlag(std::string_view s) const;
    // Set specified flag
    bool setFlag(std::string_view flag, bool state);

    /*
     * Scoring
     */
    public:
    // Evaluate the node and return its score
    int score(const SpeciesAtom *i, std::vector<const SpeciesAtom *> &matchPath) const;
};

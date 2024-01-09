// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "classes/speciesAtom.h"
#include "classes/speciesBond.h"
#include "neta/node.h"
#include <memory>
#include <vector>

// Forward Declarations
class ForcefieldAtomType;
class NETADefinition;

// NETA Connection Node
class NETAConnectionNode : public NETANode
{
    public:
    NETAConnectionNode(NETADefinition *parent, std::vector<Elements::Element> targetElements = {},
                       std::vector<std::reference_wrapper<const ForcefieldAtomType>> targetAtomTypes = {},
                       SpeciesBond::BondType bt = SpeciesBond::nBondTypes);
    ~NETAConnectionNode() override = default;

    /*
     * Atom Targets
     */
    private:
    // Array of elements that the current context atom may be
    std::vector<Elements::Element> allowedElements_;
    // Array of ForcefieldAtomTypes that the current context atom may be
    std::vector<std::reference_wrapper<const ForcefieldAtomType>> allowedAtomTypes_;
    // Type of required connection
    SpeciesBond::BondType bondType_;

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

    public:
    // Available modifiers
    enum class NETAConnectionModifier
    {
        Repeat /* 'n' - Specifies the number of matches required (default = 1) */
    };
    // Return enum options for NETAConnectionModifiers
    static EnumOptions<NETAConnectionNode::NETAConnectionModifier> modifiers();
    // Return whether the specified modifier is valid for this node
    bool isValidModifier(std::string_view s) const override;
    // Set value and comparator for specified modifier
    bool setModifier(std::string_view modifier, ComparisonOperator op, int value) override;

    /*
     * Flags
     */
    private:
    // Whether to allow the root atom of the path to be re-matched within this node
    bool allowRootMatch_;

    public:
    // Available flags
    enum class NETAConnectionFlag
    {
        Root /* 'root' - Specifies that the root atom of the current path may be re-matched */
    };
    // Return enum options for NETAConnectionFlags
    static EnumOptions<NETAConnectionNode::NETAConnectionFlag> flags();
    // Return whether the specified flag is valid for this node
    bool isValidFlag(std::string_view s) const override;
    // Set specified flag
    bool setFlag(std::string_view flag, bool state) override;

    /*
     * Scoring
     */
    public:
    // Evaluate the node and return its score
    int score(const SpeciesAtom *i, NETAMatchedGroup &matchPath) const override;
};

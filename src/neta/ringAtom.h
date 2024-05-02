// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "classes/speciesBond.h"
#include "neta/node.h"
#include <memory>
#include <vector>

// Forward Declarations
class Element;
class ForcefieldAtomType;
class NETADefinition;

// NETA Ring Atom Node
class NETARingAtomNode : public NETANode
{
    public:
    NETARingAtomNode(NETADefinition *parent, std::vector<Elements::Element> targetElements = {},
                     std::vector<std::reference_wrapper<const ForcefieldAtomType>> targetAtomTypes = {});
    ~NETARingAtomNode() override = default;

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

    public:
    // Available modifiers
    enum class NETACharacterModifier
    {
        Repeat /* 'n' - Specifies the number of matches required (default = 1) */
    };
    // Return enum options for NETACharacterModifiers
    static EnumOptions<NETARingAtomNode::NETACharacterModifier> modifiers();
    // Return whether the specified modifier is valid for this node
    bool isValidModifier(std::string_view s) const override;
    // Set value and comparator for specified modifier
    bool setModifier(std::string_view modifier, ComparisonOperator op, int value) override;
    // Test repeat count value and modifier
    bool validRepeatCount(int value) const;

    /*
     * Scoring
     */
    public:
    // Return whether we match the specified atom
    int matches(const SpeciesAtom *i, NETAMatchedGroup &matchPath) const;
    // Evaluate the node and return its score
    int score(const SpeciesAtom *i, NETAMatchedGroup &matchPath) const override;
};

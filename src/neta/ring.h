// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "classes/speciesring.h"
#include "neta/node.h"
#include <vector>

// Forward Declarations
class Element;
class ForcefieldAtomType;
class NETADefinition;

// NETA Ring Node
class NETARingNode : public NETANode
{
    public:
    NETARingNode(NETADefinition *parent);
    ~NETARingNode() = default;

    /*
     * Modifiers
     */
    private:
    // Repeat count value
    int repeatCount_;
    // Repeat count comparison operator
    NETANode::ComparisonOperator repeatCountOperator_;
    // Ring size value
    int sizeValue_;
    // Ring size value comparison operator
    NETANode::ComparisonOperator sizeValueOperator_;

    public:
    // Available modifiers
    enum class NETARingModifier
    {
        Size,  /* 'size' - Specifies size of ring (default = -1) */
        Repeat /* 'n' - Specifies the number of matches required (default = 1) */
    };
    // Return enum options for NETARingModifiers
    static EnumOptions<NETARingNode::NETARingModifier> modifiers();
    // Return whether the specified modifier is valid for this node
    bool isValidModifier(std::string_view s) const override;
    // Set value and comparator for specified modifier
    bool setModifier(std::string_view modifier, ComparisonOperator op, int value) override;

    /*
     * Scoring
     */
    private:
    // Locate rings in which the specified atom is involved
    void findRings(const SpeciesAtom *currentAtom, std::vector<SpeciesRing> &rings, std::vector<const SpeciesAtom *> &path,
                   const int minSize, const int maxSize) const;

    public:
    // Evaluate the node and return its score
    int score(const SpeciesAtom *i, std::vector<const SpeciesAtom *> &matchPath) const override;
};

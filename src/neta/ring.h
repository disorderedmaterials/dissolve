// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#pragma once

#include "classes/speciesbond.h"
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
    ~NETARingNode();

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
    enum NETARingModifier
    {
        SizeModifier,       /* 'size' - Specifies size of ring (default = -1) */
        RepeatRingModifier, /* 'n' - Specifies the number of matches required (default = 1) */
        nRingModifiers
    };
    // Return enum options for NETARingModifiers
    static EnumOptions<NETARingNode::NETARingModifier> modifiers();
    // Return whether the specified modifier is valid for this node
    bool isValidModifier(std::string_view s) const;
    // Set value and comparator for specified modifier
    bool setModifier(std::string_view modifier, ComparisonOperator op, int value);

    /*
     * Scoring
     */
    private:
    // Locate rings in which the specified atom is involved
    void findRings(const SpeciesAtom *currentAtom, List<SpeciesRing> &rings, std::vector<const SpeciesAtom *> &path,
                   const int minSize, const int maxSize) const;

    public:
    // Evaluate the node and return its score
    int score(const SpeciesAtom *i, std::vector<const SpeciesAtom*> &matchPath) const;
};

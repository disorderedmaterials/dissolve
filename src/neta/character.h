// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "classes/speciesAtom.h"
#include "neta/node.h"
#include <memory>
#include <vector>

// Forward Declarations
class ForcefieldAtomType;
class NETADefinition;

// NETA Character Node
class NETACharacterNode : public NETANode
{
    public:
    NETACharacterNode(NETADefinition *parent, std::vector<Elements::Element> targetElements = {},
                      std::vector<std::reference_wrapper<const ForcefieldAtomType>> targetAtomTypes = {});
    ~NETACharacterNode() override = default;

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
     * Scoring
     */
    public:
    // Evaluate the node and return its score
    int score(const SpeciesAtom *i, NETAMatchedGroup &matchPath) const override;
};

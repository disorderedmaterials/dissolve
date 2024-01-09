// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "classes/speciesAtom.h"
#include "neta/node.h"

// Forward Declarations
class NETADefinition;

// NETA Geometry Node
class NETAGeometryNode : public NETANode
{
    public:
    NETAGeometryNode(NETADefinition *parent);
    ~NETAGeometryNode() override = default;

    /*
     * Bond Count
     */
    private:
    // Atom Geometry
    SpeciesAtom::AtomGeometry geometry_;
    // Geometry comparison operator
    NETANode::ComparisonOperator operator_;

    public:
    // Set bond count requirements
    void set(ComparisonOperator op, SpeciesAtom::AtomGeometry geometry);

    /*
     * Scoring
     */
    public:
    // Evaluate the node and return its score
    int score(const SpeciesAtom *i, NETAMatchedGroup &matchPath) const override;
};

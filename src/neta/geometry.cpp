// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "neta/geometry.h"
#include "classes/speciesAtom.h"

NETAGeometryNode::NETAGeometryNode(NETADefinition *parent)
    : NETANode(parent, NETANode::NodeType::Geometry), geometry_(BaseAtom::AtomGeometry::Unknown),
      operator_(NETANode::ComparisonOperator::EqualTo)
{
}

/*
 * Geometry Count
 */

// Set bond count requirements
void NETAGeometryNode::set(ComparisonOperator op, BaseAtom::AtomGeometry geometry)
{
    geometry_ = geometry;
    operator_ = op;
}

/*
 * Scoring
 */

// Evaluate the node and return its score
int NETAGeometryNode::score(const BaseAtom *i, NETAMatchedGroup &matchPath) const
{
    auto result = i->isGeometry(geometry_);

    if ((operator_ == NETANode::ComparisonOperator::EqualTo && !result) ||
        (operator_ == NETANode::ComparisonOperator::NotEqualTo && result))
        return NETANode::NoMatch;

    return 1;
}

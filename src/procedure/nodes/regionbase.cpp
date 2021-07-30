// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "procedure/nodes/regionbase.h"
#include "classes/box.h"
#include "classes/configuration.h"

constexpr double voxelSize = 1.0;

RegionProcedureNodeBase::RegionProcedureNodeBase(ProcedureNode::NodeType nodeType)
    : ProcedureNode(nodeType, ProcedureNode::NodeClass::Region)
{
}

/*
 * Identity
 */

// Return whether specified context is relevant for this node type
bool RegionProcedureNodeBase::isContextRelevant(ProcedureNode::NodeContext context)
{
    return (context == ProcedureNode::GenerationContext);
}

// Return whether a name for the node must be provided
bool RegionProcedureNodeBase::mustBeNamed() const { return true; }

/*
 * Region Data
 */

// Return current region data
const Region &RegionProcedureNodeBase::region() const { return region_; }

// Regenerate and return region data
const Region RegionProcedureNodeBase::generateRegion(const Configuration *cfg) const
{
    Region newRegion;
    newRegion.generate(cfg, voxelSize, [&](const Configuration *cfg, Vec3<double> r) { return isVoxelValid(cfg, r); });
    return newRegion;
}

/*
 * Execute
 */

// Execute node, targetting the supplied Configuration
bool RegionProcedureNodeBase::execute(ProcessPool &procPool, Configuration *cfg, std::string_view prefix,
                                      GenericList &targetList)
{
    return region_.generate(cfg, voxelSize, [&](const Configuration *cfg, Vec3<double> r) { return isVoxelValid(cfg, r); });
}

// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "procedure/nodes/regionbase.h"
#include "keywords/bool.h"
#include "keywords/double.h"

RegionProcedureNodeBase::RegionProcedureNodeBase(ProcedureNode::NodeType nodeType)
    : ProcedureNode(nodeType, ProcedureNode::NodeClass::Region)
{
    keywords_.add("Control", new DoubleKeyword(1.0, 0.1), "VoxelSize",
                  "Voxel size (length) guiding the coarseness / detail of the region");
    keywords_.add("Control", new BoolKeyword(false), "Invert", "Invert the logic used to determine free space in the region");
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
    auto validState = !keywords_.asBool("Invert");
    Region newRegion;
    newRegion.generate(cfg, keywords_.asDouble("VoxelSize"),
                       [&](const Configuration *cfg, Vec3<double> r) { return isVoxelValid(cfg, r) == validState; });
    return newRegion;
}

/*
 * Execute
 */

// Execute node, targetting the supplied Configuration
bool RegionProcedureNodeBase::execute(ProcessPool &procPool, Configuration *cfg, std::string_view prefix,
                                      GenericList &targetList)
{
    auto validState = !keywords_.asBool("Invert");
    return region_.generate(cfg, keywords_.asDouble("VoxelSize"),
                            [&](const Configuration *cfg, Vec3<double> r) { return isVoxelValid(cfg, r) == validState; });
}

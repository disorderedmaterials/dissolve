// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "procedure/nodes/regionbase.h"
#include "keywords/bool.h"
#include "keywords/double.h"

RegionProcedureNodeBase::RegionProcedureNodeBase(ProcedureNode::NodeType nodeType)
    : ProcedureNode(nodeType, ProcedureNode::NodeClass::Region)
{
    keywords_.add<DoubleKeyword>("Control", "VoxelSize", "Voxel size (length) guiding the coarseness / detail of the region",
                                 voxelSize_, 0.1);
    keywords_.add<BoolKeyword>("Control", "Invert", "Invert the logic used to determine free space in the region", invert_);
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
    newRegion.generate(cfg, voxelSize_,
                       [&](const Configuration *cfg, Vec3<double> r) { return isVoxelValid(cfg, r) != invert_; });
    return newRegion;
}

/*
 * Execute
 */

// Execute node
bool RegionProcedureNodeBase::execute(const ProcedureContext &procedureContext)
{
    return region_.generate(procedureContext.configuration(), voxelSize_,
                            [&](const Configuration *cfg, Vec3<double> r) { return isVoxelValid(cfg, r) != invert_; });
}

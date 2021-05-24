// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "procedure/nodes/regionbase.h"
#include "classes/box.h"
#include "classes/configuration.h"

constexpr double voxelSize = 2.0;

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

// Return random fractional coordinate inside region
Vec3<double> RegionProcedureNodeBase::randomFractionalCoordinate() const
{
    // Select a random voxel
    auto &voxel = freeVoxels_[DissolveMath::randomi(freeVoxels_.size())];

    // Generate random point in fractional coordinates within the voxel
    return Vec3<double>((voxel.first.x + DissolveMath::random()) * voxelSizeFrac_.x,
                        (voxel.first.y + DissolveMath::random()) * voxelSizeFrac_.y,
                        (voxel.first.z + DissolveMath::random()) * voxelSizeFrac_.z);
}

// Return whether specified fractional coordinate is inside a valid cell of the region
bool RegionProcedureNodeBase::validFractionalCoordinate(const Vec3<double> &rFrac) const
{
    return voxelMap_[{rFrac.x / voxelSizeFrac_.x, rFrac.y / voxelSizeFrac_.y, rFrac.z / voxelSizeFrac_.z}].second;
}

/*
 * Execute
 */

// Execute node, targetting the supplied Configuration
bool RegionProcedureNodeBase::execute(ProcessPool &procPool, Configuration *cfg, std::string_view prefix,
                                      GenericList &targetList)
{
    // Copy Box pointer from Configuration
    box_ = cfg->box();

    // Determine number of divisions along each cell axis, and set fractional voxel size
    for (auto n = 0; n < 3; ++n)
        nVoxels_.set(n, cfg->box()->axisLength(n) / voxelSize);
    voxelSizeFrac_.set(1.0 / nVoxels_.x, 1.0 / nVoxels_.y, 1.0 / nVoxels_.z);

    // Initialise 3D map and determine valid voxels
    voxelMap_.initialise(nVoxels_.x, nVoxels_.y, nVoxels_.z);
    for (auto x = 0; x < nVoxels_.x; ++x)
        for (auto y = 0; y < nVoxels_.y; ++y)
            for (auto z = 0; z < nVoxels_.z; ++z)
                voxelMap_[{x, y, z}] = {
                    {x, y, z},
                    isVoxelValid(cfg, box_->fracToReal({(x + 0.5) * voxelSizeFrac_.x, (y + 0.5) * voxelSizeFrac_.y,
                                                        (z + 0.5) * voxelSizeFrac_.z}))};

    // Create linear array of all available voxels
    auto nFreeVoxels = std::count_if(voxelMap_.begin(), voxelMap_.end(), [](const auto &voxel) { return voxel.second; });
    freeVoxels_.clear();
    freeVoxels_.resize(nFreeVoxels);
    std::copy_if(voxelMap_.begin(), voxelMap_.end(), freeVoxels_.begin(), [](const auto &voxel) { return voxel.second; });

    return nFreeVoxels > 0;
}

// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "generator/generalRegion.h"
#include "classes/configuration.h"
#include "keywords/double.h"

/*
 * General Region Voxel Kernel
 */

GeneralRegionVoxelKernel::GeneralRegionVoxelKernel(double toleranceSquared) : toleranceSquared_(toleranceSquared) {}

// Return whether voxel centred at supplied real coordinates is valid
bool GeneralRegionVoxelKernel::isVoxelValid(const Configuration *cfg, const Vec3<double> &r) const
{
    // If any atom in the Configuration is less than some tolerance value to this coordinate, invalidate the voxel
    return !std::any_of(cfg->atoms().begin(), cfg->atoms().end(),
                        [&](const auto &i) { return cfg->box()->minimumDistanceSquared(i.r(), r) <= toleranceSquared_; });
}

/*
 * General Region
 */

GeneralRegionProcedureNode::GeneralRegionProcedureNode() : RegionProcedureNodeBase(ProcedureNode::NodeType::GeneralRegion)
{
    keywords_.setOrganisation("Options", "Definition");
    keywords_.add<DoubleKeyword>("Tolerance", "Distance threshold for avoiding existing atoms in the configuration", tolerance_,
                                 0.1);
}

// Return a new voxel check kernel
std::shared_ptr<VoxelKernel> GeneralRegionProcedureNode::createVoxelKernel()
{
    return std::make_shared<GeneralRegionVoxelKernel>(tolerance_ * tolerance_);
}

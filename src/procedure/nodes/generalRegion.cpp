// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "procedure/nodes/generalRegion.h"
#include "classes/configuration.h"
#include "keywords/double.h"

// Return whether voxel centred at supplied real coordinates is valid
bool GeneralRegionVoxelKernel::isVoxelValid(const Configuration *cfg, const Vec3<double> &r) const
{
    // TODO
}


GeneralRegionProcedureNode::GeneralRegionProcedureNode() : RegionProcedureNodeBase(ProcedureNode::NodeType::GeneralRegion)
{
    keywords_.setOrganisation("Options", "Definition");
    keywords_.add<DoubleKeyword>("Tolerance", "Distance threshold for avoiding existing atoms in the configuration", tolerance_,
                                 0.1);
}

/*
 * Region Data
 */

// Return a new voxel check kernel
std::unique_ptr<VoxelKernel> GeneralRegionProcedureNode::createVoxelKernel() { return std::make_unique<GeneralRegionVoxelKernel>(); }

// Return whether voxel centred at supplied real coordinates is valid
bool GeneralRegionProcedureNode::isVoxelValid(const Configuration *cfg, const Vec3<double> &r) const
{
    // If any atom in the Configuration is less than some tolerance value to this coordinate, invalidate the voxel
    return !std::any_of(cfg->atoms().begin(), cfg->atoms().end(),
                        [&](const auto &i) { return cfg->box()->minimumDistanceSquared(i.r(), r) <= toleranceSquared_; });
}

/*
 * Execute
 */

// Prepare any necessary data, ready for execution
bool GeneralRegionProcedureNode::prepare(const ProcedureContext &procedureContext)
{
    // Retrieve keyword values
    toleranceSquared_ = tolerance_ * tolerance_;

    return true;
}

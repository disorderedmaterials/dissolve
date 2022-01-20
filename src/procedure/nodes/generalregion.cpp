// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "procedure/nodes/generalregion.h"
#include "classes/configuration.h"
#include "keywords/double.h"

GeneralRegionProcedureNode::GeneralRegionProcedureNode() : RegionProcedureNodeBase(ProcedureNode::NodeType::GeneralRegion)
{
    keywords_.add("Control", new DoubleKeyword(2.0), "Tolerance",
                  "Distance threshold for avoiding existing atoms in the configuration");
}

/*
 * Region Data
 */

// Return whether voxel centred at supplied real coordinates is valid
bool GeneralRegionProcedureNode::isVoxelValid(const Configuration *cfg, const Vec3<double> &vCentre) const
{
    const Box *box = cfg->box();

    // If any atom in the Configuration is less than some tolerance value to this coordinate, invalidate the voxel
    return !std::any_of(cfg->atoms().begin(), cfg->atoms().end(),
                        [&](const auto &i) { return box->minimumDistanceSquared(i, vCentre) <= toleranceSquared_; });
}

/*
 * Execute
 */

// Prepare any necessary data, ready for execution
bool GeneralRegionProcedureNode::prepare(Configuration *cfg, std::string_view prefix, GenericList &targetList)
{
    // Retrieve keyword values
    toleranceSquared_ = keywords_.asDouble("Tolerance");
    toleranceSquared_ *= toleranceSquared_;

    return true;
}

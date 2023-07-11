// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "procedure/nodeValue.h"
#include "procedure/nodes/regionBase.h"

// Custom Region
class CustomRegionProcedureNode : public RegionProcedureNodeBase
{
    public:
    CustomRegionProcedureNode();
    ~CustomRegionProcedureNode() override = default;

    /*
     * Control
     */
    private:
    // Local variables, set when checking voxels
    std::shared_ptr<ExpressionVariable> x_, y_, z_, xFrac_, yFrac_, zFrac_;
    // Expression describing region
    NodeValue expression_;
    // Minimum threshold value for function
    double minimumValue_{0.0};
    // Maximum threshold value for function
    double maximumValue_{1.0};

    /*
     * Region Data
     */
    public:
    // Return whether voxel centred at supplied real coordinates is valid
    bool isVoxelValid(const Configuration *cfg, const Vec3<double> &r) const override;
};
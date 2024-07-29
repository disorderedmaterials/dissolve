// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "generator/nodeValue.h"
#include "generator/regionBase.h"

// Custom Region Voxel Kernel
class CustomRegionVoxelKernel : public VoxelKernel
{
    public:
    explicit CustomRegionVoxelKernel(std::string_view expressionString = "",
                                     std::vector<std::shared_ptr<ExpressionVariable>> = {}, double minimumValue = 0.0,
                                     double maximumValue = 1.0);

    protected:
    // Local variables, set when checking voxels
    std::shared_ptr<ExpressionVariable> x_, y_, z_, xFrac_, yFrac_, zFrac_;
    // Expression describing region
    NodeValue expression_;
    // Minimum threshold value for function
    double minimumValue_{0.0};
    // Maximum threshold value for function
    double maximumValue_{1.0};

    public:
    // Return whether voxel centred at supplied real coordinates is valid
    bool isVoxelValid(const Configuration *cfg, const Vec3<double> &r) const override;
};

// Custom Region
class CustomRegionGeneratorNode : public RegionGeneratorNodeBase, CustomRegionVoxelKernel
{
    public:
    CustomRegionGeneratorNode();
    ~CustomRegionGeneratorNode() override = default;

    /*
     * Region Data
     */
    protected:
    // Return a new voxel check kernel
    std::shared_ptr<VoxelKernel> createVoxelKernel() override;
};

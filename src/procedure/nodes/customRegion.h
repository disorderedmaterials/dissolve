// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "procedure/nodeValue.h"
#include "procedure/nodes/regionBase.h"

// Custom Region Voxel Kernel
class CustomRegionVoxelKernel : public VoxelKernel
{
    private:
    // Local variables, set when checking voxels
    std::shared_ptr<ExpressionVariable> x_, y_, z_, xFrac_, yFrac_, zFrac_;
    // Expression describing region
    NodeValue expression_;
    // Minimum threshold value for function
    double minimumValue_{0.0};
    // Maximum threshold value for function
    double maximumValue_{1.0};

    public:
    // Set up necessary data for the kernel
    void initialise(std::string_view expressionString, double minimumValue, double maximumValue);
    // Return whether voxel centred at supplied real coordinates is valid
    bool isVoxelValid(const Configuration *cfg, const Vec3<double> &r) const override;
};

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
    protected:
    // Return a new voxel check kernel
    std::unique_ptr<VoxelKernel> createVoxelKernel() override;

    public:
    // Return whether voxel centred at supplied real coordinates is valid
    bool isVoxelValid(const Configuration *cfg, const Vec3<double> &r) const override;
};

// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "procedure/nodes/regionBase.h"

// Custom Region Voxel Kernel
class CylindricalRegionVoxelKernel : public VoxelKernel
{
    public:
    explicit CylindricalRegionVoxelKernel(Vec3<double> originFrac = {0.0, 0.0, 0.0}, double radius = 5.0,
                                          Vec3<double> vector = {0.0, 0.0, 1.0});

    protected:
    // Origin of vector in fractional coordinates
    Vec3<double> originFrac_{0.0, 0.0, 0.0};
    // Radius of cylindrical region
    double radius_{5.0};
    // Cylinder vector
    Vec3<double> vector_{0.0, 0.0, 1.0};

    public:
    // Return whether voxel centred at supplied real coordinates is valid
    bool isVoxelValid(const Configuration *cfg, const Vec3<double> &r) const override;
};

// Cylindrical Region
class CylindricalRegionProcedureNode : public RegionProcedureNodeBase, CylindricalRegionVoxelKernel
{
    public:
    CylindricalRegionProcedureNode();
    ~CylindricalRegionProcedureNode() override = default;

    /*
     * Region Data
     */
    protected:
    // Return a new voxel check kernel
    std::shared_ptr<VoxelKernel> createVoxelKernel() override;
};

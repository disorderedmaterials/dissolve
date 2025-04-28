// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "generator/regionBase.h"

// Custom Region Voxel Kernel
class CylindricalRegionVoxelKernel : public VoxelKernel
{
    public:
    explicit CylindricalRegionVoxelKernel(Vector3 originFrac = {0.0, 0.0, 0.0}, double radius = 5.0,
                                          Vector3 vector = {0.0, 0.0, 1.0});

    protected:
    // Origin of vector in fractional coordinates
    Vector3 originFrac_{0.0, 0.0, 0.0};
    // Radius of cylindrical region
    double radius_{5.0};
    // Cylinder vector
    Vector3 vector_{0.0, 0.0, 1.0};

    public:
    // Return whether voxel centred at supplied real coordinates is valid
    bool isVoxelValid(const Configuration *cfg, const Vector3 &r) const override;
};

// Cylindrical Region
class CylindricalRegionGeneratorNode : public RegionGeneratorNodeBase, CylindricalRegionVoxelKernel
{
    public:
    CylindricalRegionGeneratorNode();
    ~CylindricalRegionGeneratorNode() override = default;

    /*
     * Region Data
     */
    protected:
    // Return a new voxel check kernel
    std::shared_ptr<VoxelKernel> createVoxelKernel() override;
};

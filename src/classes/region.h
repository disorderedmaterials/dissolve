// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "classes/array3DIterator.h"
#include "classes/box.h"
#include "classes/configuration.h"
#include "math/vector3.h"
#include "templates/algorithms.h"
#include "templates/array3D.h"
#include "templates/combinable.h"
#include "templates/parallelDefs.h"
#include <functional>
#include <vector>

// Forward Declarations
class Configuration;

// Voxel Kernel
class VoxelKernel
{
    public:
    // Return whether voxel centred at supplied real coordinates is valid
    virtual bool isVoxelValid(const Configuration *cfg, const Vector3 &r) const = 0;
};

// Region Data
class Region
{
    public:
    Region();
    ~Region() = default;

    private:
    // Box from target Configuration
    const Box *box_;
    // 3D map of available voxels
    Array3D<std::pair<Vector3i, bool>> voxelMap_;
    // Number of voxels along each cell axis
    Vector3i nVoxels_;
    // Fractional voxel size
    Vector3 voxelSizeFrac_;
    // Lower-left corner voxel indices of free regions
    std::vector<std::pair<Vector3i, bool>> freeVoxels_;

    private:
    // Generate voxel combinable
    static dissolve::CombinableFunctor<std::shared_ptr<VoxelKernel>>
    createCombinableVoxelKernel(std::function<std::shared_ptr<VoxelKernel>(void)> kernelGenerator)
    {
        return kernelGenerator;
    }

    public:
    // Generate region information
    bool generate(const Configuration *cfg, double voxelSize, bool invert,
                  const std::function<std::shared_ptr<VoxelKernel>(void)> &kernelGenerator);
    // Return whether the region is valid
    bool isValid() const;
    // Return the fraction free voxels in the region
    double freeVoxelFraction() const;
    // Return random coordinate inside region
    Vector3 randomCoordinate() const;
    // Return whether specified coordinate is inside the region
    bool validCoordinate(Vector3 r) const;
};

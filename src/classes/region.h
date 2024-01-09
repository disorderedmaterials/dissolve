// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "classes/array3DIterator.h"
#include "classes/box.h"
#include "classes/configuration.h"
#include "templates/algorithms.h"
#include "templates/array3D.h"
#include "templates/combinable.h"
#include "templates/parallelDefs.h"
#include "templates/vector3.h"
#include <functional>
#include <vector>

// Forward Declarations
class Configuration;

// Voxel Kernel
class VoxelKernel
{
    public:
    // Return whether voxel centred at supplied real coordinates is valid
    virtual bool isVoxelValid(const Configuration *cfg, const Vec3<double> &r) const = 0;
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
    Array3D<std::pair<Vec3<int>, bool>> voxelMap_;
    // Number of voxels along each cell axis
    Vec3<int> nVoxels_;
    // Fractional voxel size
    Vec3<double> voxelSizeFrac_;
    // Lower-left corner voxel indices of free regions
    std::vector<std::pair<Vec3<int>, bool>> freeVoxels_;

    private:
    // Generate voxel combinable
    static dissolve::CombinableFunctor<std::shared_ptr<VoxelKernel>>
    createCombinableVoxelKernel(std::function<std::shared_ptr<VoxelKernel>(void)> kernelGenerator)
    {
        return kernelGenerator;
    }

    public:
    // Generate region information
    bool generate(const Configuration *cfg, double voxelSize,
                  const std::function<std::shared_ptr<VoxelKernel>(void)> &kernelGenerator);
    // Return whether the region is valid
    bool isValid() const;
    // Return the fraction free voxels in the region
    double freeVoxelFraction() const;
    // Return random coordinate inside region
    Vec3<double> randomCoordinate() const;
    // Return whether specified coordinate is inside the region
    bool validCoordinate(Vec3<double> r) const;
};

// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#pragma once

#include "classes/box.h"
#include "classes/configuration.h"
#include "templates/array3d.h"
#include "templates/vector3.h"
#include <vector>

// Forward Declarations
class Configuration;

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

    public:
    // Generate region information
    template <class Lam> bool generate(const Configuration *cfg, double voxelSize, Lam voxelCheckFunction)
    {
        box_ = cfg->box();

        // Set fractional voxel sizes
        for (auto n = 0; n < 3; ++n)
            nVoxels_.set(n, std::max(int(box_->axisLength(n) / voxelSize), 1));
        voxelSizeFrac_.set(1.0 / nVoxels_.x, 1.0 / nVoxels_.y, 1.0 / nVoxels_.z);

        // Initialise 3D map and determine valid voxels
        voxelMap_.initialise(nVoxels_.x, nVoxels_.y, nVoxels_.z);
        for (auto x = 0; x < nVoxels_.x; ++x)
            for (auto y = 0; y < nVoxels_.y; ++y)
                for (auto z = 0; z < nVoxels_.z; ++z)
                    voxelMap_[{x, y, z}] = {
                        Vec3<int>(x, y, z),
                        voxelCheckFunction(cfg, box_->getReal({(x + 0.5) * voxelSizeFrac_.x, (y + 0.5) * voxelSizeFrac_.y,
                                                               (z + 0.5) * voxelSizeFrac_.z}))};

        // Create linear vector of all available voxels
        auto nFreeVoxels = std::count_if(voxelMap_.begin(), voxelMap_.end(), [](const auto &voxel) { return voxel.second; });
        freeVoxels_.clear();
        freeVoxels_.resize(nFreeVoxels);
        std::copy_if(voxelMap_.begin(), voxelMap_.end(), freeVoxels_.begin(), [](const auto &voxel) { return voxel.second; });

        return nFreeVoxels > 0;
    }
    // Return whether the region is valid
    bool isValid() const;
    // Return the fraction free voxels in the region
    double freeVoxelFraction() const;
    // Return random coordinate inside region
    Vec3<double> randomCoordinate() const;
    // Return whether specified coordinate is inside the region
    bool validCoordinate(Vec3<double> r) const;
};

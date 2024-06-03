// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/region.h"
#include "classes/box.h"
#include "classes/configuration.h"

Region::Region() : box_(nullptr) {}

// Generate region information
bool Region::generate(const Configuration *cfg, double voxelSize, bool invert,
                      const std::function<std::shared_ptr<VoxelKernel>(void)> &kernelGenerator)
{
    box_ = cfg->box();

    // Set fractional voxel sizes
    for (auto n = 0; n < 3; ++n)
        nVoxels_.set(n, std::max(int(box_->axisLength(n) / voxelSize), 1));
    voxelSizeFrac_.set(1.0 / nVoxels_.x, 1.0 / nVoxels_.y, 1.0 / nVoxels_.z);

    // Initialise 3D map and determine valid voxels
    voxelMap_.initialise(nVoxels_.x, nVoxels_.y, nVoxels_.z);

    // Create a voxel combinable and check function
    auto voxelCombinable = Region::createCombinableVoxelKernel(kernelGenerator);
    auto voxelCheckFunction = [&](auto triplet, auto x, auto y, auto z)
    {
        voxelMap_[triplet] = {Vec3<int>(x, y, z),
                              invert != voxelCombinable.local()->isVoxelValid(
                                            cfg, box_->getReal({(x + 0.5) * voxelSizeFrac_.x, (y + 0.5) * voxelSizeFrac_.y,
                                                                (z + 0.5) * voxelSizeFrac_.z}))};
    };

    // Iterate over voxels
    dissolve::for_each_triplet(ParallelPolicies::par, voxelMap_.beginIndices(), voxelMap_.endIndices(), voxelCheckFunction);

    // Create linear vector of all available voxels
    auto nFreeVoxels = std::count_if(voxelMap_.begin(), voxelMap_.end(), [](const auto &voxel) { return voxel.second; });
    freeVoxels_.clear();
    freeVoxels_.resize(nFreeVoxels);
    std::copy_if(voxelMap_.begin(), voxelMap_.end(), freeVoxels_.begin(), [](const auto &voxel) { return voxel.second; });

    return nFreeVoxels > 0;
}

// Return whether the region is valid
bool Region::isValid() const { return !voxelMap_.empty() && !freeVoxels_.empty(); }

// Return the fraction free voxels in the region
double Region::freeVoxelFraction() const { return double(freeVoxels_.size()) / double(voxelMap_.linearArraySize()); }

// Return random coordinate inside region
Vec3<double> Region::randomCoordinate() const
{
    // Select a random voxel
    auto &voxel = freeVoxels_[DissolveMath::randomi(freeVoxels_.size())];

    // Generate random point in fractional coordinates within the voxel
    Vec3<double> r = {(voxel.first.x + DissolveMath::random()) * voxelSizeFrac_.x,
                      (voxel.first.y + DissolveMath::random()) * voxelSizeFrac_.y,
                      (voxel.first.z + DissolveMath::random()) * voxelSizeFrac_.z};
    box_->toReal(r);
    return r;
}

// Return whether specified coordinate is inside the region
bool Region::validCoordinate(Vec3<double> r) const
{
    box_->toFractional(r);
    return voxelMap_[std::tuple{r.x / voxelSizeFrac_.x, r.y / voxelSizeFrac_.y, r.z / voxelSizeFrac_.z}].second;
}

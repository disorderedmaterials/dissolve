// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/region.h"
#include "classes/box.h"
#include "classes/configuration.h"

Region::Region() : box_(nullptr) {}

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
    return voxelMap_[{r.x / voxelSizeFrac_.x, r.y / voxelSizeFrac_.y, r.z / voxelSizeFrac_.z}].second;
}

// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "procedure/nodes/cylindricalRegion.h"
#include "classes/configuration.h"
#include "keywords/double.h"
#include "keywords/vec3Double.h"

// Return whether voxel centred at supplied real coordinates is valid
bool CylindricalRegionVoxelKernel::isVoxelValid(const Configuration *cfg, const Vec3<double> &r) const
{
    // TODO
}

CylindricalRegionProcedureNode::CylindricalRegionProcedureNode()
    : RegionProcedureNodeBase(ProcedureNode::NodeType::CylindricalRegion)
{
    keywords_.setOrganisation("Options", "Definition");
    keywords_.add<Vec3DoubleKeyword>("OriginFrac", "Origin of vector in fractional coordinates", originFrac_,
                                     Vec3Labels::XYZLabels);
    keywords_.add<DoubleKeyword>("Radius", "Cylinder radius", radius_, 1.0e-3);
    keywords_.add<Vec3DoubleKeyword>("Vector", "Cylinder vector", vector_, Vec3Labels::XYZLabels);
}

/*
 * Region Data
 */

// Return a new voxel check kernel
std::shared_ptr<VoxelKernel> CylindricalRegionProcedureNode::createVoxelKernel()
{
    return std::make_shared<CylindricalRegionVoxelKernel>();
}

// Return whether voxel centred at supplied real coordinates is valid
bool CylindricalRegionProcedureNode::isVoxelValid(const Configuration *cfg, const Vec3<double> &r) const
{
    auto l0 = originFrac_;
    cfg->box()->toReal(l0);
    const auto l1 = l0 + vector_;
    const auto denominator = (l1 - l0).magnitude();

    auto p0 = cfg->box()->minimumImage(r, l0);
    auto num = ((p0 - l0) * (p0 - l1)).magnitude();

    // Check distance vs cylinder radius
    return (num / denominator) <= radius_;
}

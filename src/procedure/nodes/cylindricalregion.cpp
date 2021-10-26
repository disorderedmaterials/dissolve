// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "procedure/nodes/cylindricalregion.h"
#include "classes/configuration.h"
#include "keywords/double.h"
#include "keywords/vec3double.h"

CylindricalRegionProcedureNode::CylindricalRegionProcedureNode()
    : RegionProcedureNodeBase(ProcedureNode::NodeType::CylindricalRegion)
{
    keywords_.add("Control", new Vec3DoubleKeyword({0.0, 0.0, 0.0}, Vec3Labels::XYZLabels), "OriginFrac",
                  "Origin of vector in fractional coordinates");
    keywords_.add<DoubleKeyword>("Control", "Radius", "Cylinder radius", radius_, 1.0e-3);
    keywords_.add("Control", new Vec3DoubleKeyword({0.0, 0.0, 1.0}, Vec3Labels::XYZLabels), "Vector", "Cylinder vector");
}

/*
 * Region Data
 */

// Return whether voxel centred at supplied real coordinates is valid
bool CylindricalRegionProcedureNode::isVoxelValid(const Configuration *cfg, const Vec3<double> &r) const
{
    auto l0 = keywords_.asVec3Double("OriginFrac");
    cfg->box()->toReal(l0);
    const auto l1 = l0 + keywords_.asVec3Double("Vector");
    const auto denominator = (l1 - l0).magnitude();

    auto p0 = cfg->box()->minimumImage(r, l0);
    auto num = ((p0 - l0) * (p0 - l1)).magnitude();

    // Check distance vs cylinder radius
    return (num / denominator) <= radius_;
}

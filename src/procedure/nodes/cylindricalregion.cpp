// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "procedure/nodes/cylindricalregion.h"
#include "classes/configuration.h"
#include "keywords/double.h"
#include "keywords/vec3double.h"

CylindricalRegionProcedureNode::CylindricalRegionProcedureNode()
    : RegionProcedureNodeBase(ProcedureNode::NodeType::CylindricalRegion)
{
    keywords_.add<Vec3DoubleKeyword>("Control", "OriginFrac", "Origin of vector in fractional coordinates", originFrac_,
                                     Vec3Labels::XYZLabels);
    keywords_.add<DoubleKeyword>("Control", "Radius", "Cylinder radius", radius_, 1.0e-3);
    keywords_.add<Vec3DoubleKeyword>("Control", "Vector", "Cylinder vector", vector_, Vec3Labels::XYZLabels);
}

/*
 * Region Data
 */

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

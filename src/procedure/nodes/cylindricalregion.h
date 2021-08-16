// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "procedure/nodes/regionbase.h"

// Cylindrical Region
class CylindricalRegionProcedureNode : public RegionProcedureNodeBase
{
    public:
    CylindricalRegionProcedureNode();
    ~CylindricalRegionProcedureNode() override = default;

    /*
     * Region Data
     */
    public:
    // Return whether voxel centred at supplied real coordinates is valid
    bool isVoxelValid(const Configuration *cfg, const Vec3<double> &r) const;
};

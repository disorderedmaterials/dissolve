// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#pragma once

#include "procedure/nodes/regionbase.h"

// General Region
class GeneralRegionProcedureNode : public RegionProcedureNodeBase
{
    public:
    GeneralRegionProcedureNode();
    ~GeneralRegionProcedureNode() override = default;

    /*
     * Control
     */
    private:
    // Distance tolerance (threshold) for avoiding existing atoms
    double tolerance_;
    // Squared tolerance
    double toleranceSquared_;

    /*
     * Region Data
     */
    public:
    // Return whether voxel centred at supplied real coordinates is valid
    bool isVoxelValid(const Configuration *cfg, const Vec3<double> &r) const;

    /*
     * Execute
     */
    public:
    // Prepare any necessary data, ready for execution
    bool prepare(Configuration *cfg, std::string_view prefix, GenericList &targetList) override;
};

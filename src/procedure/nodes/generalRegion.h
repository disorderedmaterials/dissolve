// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "procedure/nodes/regionBase.h"

// General Region Voxel Kernel
class GeneralRegionVoxelKernel : public VoxelKernel
{
    public:
    GeneralRegionVoxelKernel(double toleranceSquared);

    private:
    // Squared tolerance
    double toleranceSquared_;

    public:
    // Return whether voxel centred at supplied real coordinates is valid
    bool isVoxelValid(const Configuration *cfg, const Vec3<double> &r) const override;
};

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
    double tolerance_{1.0};
    // Squared tolerance
    double toleranceSquared_{1.0};

    /*
     * Region Data
     */
    protected:
    // Return a new voxel check kernel
    std::shared_ptr<VoxelKernel> createVoxelKernel() override;

    /*
     * Execute
     */
    public:
    // Prepare any necessary data, ready for execution
    bool prepare(const ProcedureContext &procedureContext) override;
};

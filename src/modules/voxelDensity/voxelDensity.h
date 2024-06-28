// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "module/module.h"

// VoxelDensity Module
class VoxelDensityModule : public Module
{
    public:
    VoxelDensityModule();
    ~VoxelDensityModule() override = default;

    /*
     * Definition
     */
    private:
    // Target configuration
    Configuration *targetConfiguration_{nullptr};
    // 3d bounds for unit cell region
    Vec3<double> unitCellVoxelRegion_;
    // Vector of species by which analysis is restricted
    std::vector<const Species *> restrictToSpecies_;
    // Analyse atomic number of unit cell region
    bool atomicNumberSelected_{false};
    // Analyse mass of unit cell region
    bool massSelected_{false};
    // Analyse scattering length density of unit cell region
    bool scatteringLengthDensitySelected_{false};

    /*
     * Processing
     */
    private:
    // Run main processing
    Module::ExecutionResult process(ModuleContext &moduleContext) override;
};
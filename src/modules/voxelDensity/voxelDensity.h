// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "base/enumOptions.h"
#include "module/module.h"

class Configuration;

// VoxelDensity Module
class VoxelDensityModule : public Module
{
    public:
    VoxelDensityModule();
    ~VoxelDensityModule() override = default;

    /*
     * Definition
     */
    public:
    // Target property
    enum class TargetPropertyType
    {
        Mass,
        AtomicNumber,
        ScatteringLengthDensity
    };
    // Return enum option info for TargetPropertyType
    static EnumOptions<VoxelDensityModule::TargetPropertyType> targetPropertyTypes();

    private:
    // Target configuration
    Configuration *targetConfiguration_{nullptr};
    // 3d bounds for unit cell region
    Vec3<double> unitCellVoxelRegion_;
    // Vector of species by which analysis is restricted
    std::vector<const Species *> restrictToSpecies_;
    // Target property for analysis
    VoxelDensityModule::TargetPropertyType targetProperty_;

    /*
     * Processing
     */
    private:
    // Run main processing
    Module::ExecutionResult process(ModuleContext &moduleContext) override;
};
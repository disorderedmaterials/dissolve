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
    // Vector of species by which analysis is restricted
    std::vector<const Species *> restrictToSpecies_;
    // Target property for analysis
    VoxelDensityModule::TargetPropertyType targetProperty_;
    // Density
    int numPoints_;  
    /*
    // Range (min, max, binwidth) of x distance binning
    Vec3<double> xAxisRange_{0.0, 10.0, 0.05};
    // Range (min, max, binwidth) of y distance binning
    Vec3<double> yAxisRange_{0.0, 10.0, 0.05};
    // Range (min, max, binwidth) of z distance binning
    Vec3<double> zAxisRange_{0.0, 10.0, 0.05};
    */

    /*
     * Processing
     */
    private:
    // Run main processing
    Module::ExecutionResult process(ModuleContext &moduleContext) override;
};
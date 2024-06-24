// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

/*
#include "io/import/trajectory.h"
*/
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

    /*
     * Processing
     */
    private:
    // Run main processing
    Module::ExecutionResult process(ModuleContext &moduleContext) override;
};
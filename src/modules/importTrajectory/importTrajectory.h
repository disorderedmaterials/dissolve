// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "io/import/trajectory.h"
#include "module/module.h"

// Import Trajectory Module
class ImportTrajectoryModule : public Module
{
    public:
    ImportTrajectoryModule();
    ~ImportTrajectoryModule() override = default;

    /*
     * Definition
     */
    private:
    // Target configuration
    Configuration *targetConfiguration_{nullptr};
    // Trajectory file source
    TrajectoryImportFileFormat trajectoryFormat_;

    /*
     * Processing
     */
    private:
    // Run main processing
    Module::ExecutionResult process(ModuleContext &moduleContext) override;
};

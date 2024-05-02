// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "io/export/trajectory.h"
#include "module/module.h"

// Export Trajectory Module
class ExportTrajectoryModule : public Module
{
    public:
    ExportTrajectoryModule();
    ~ExportTrajectoryModule() override = default;

    /*
     * Definition
     */
    private:
    // Target configuration
    Configuration *targetConfiguration_{nullptr};
    // Filename and format for trajectory export
    TrajectoryExportFileFormat trajectoryFormat_;

    /*
     * Processing
     */
    private:
    // Run main processing
    Module::ExecutionResult process(ModuleContext &moduleContext) override;
};

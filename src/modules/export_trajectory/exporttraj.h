// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

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
    bool process(Dissolve &dissolve, ProcessPool &procPool) override;
};

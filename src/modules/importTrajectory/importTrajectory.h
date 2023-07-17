// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

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
    enum executionResult process(Dissolve &dissolve, const ProcessPool &procPool) override;
};

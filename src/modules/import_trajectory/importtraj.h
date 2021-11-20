// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

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

    public:
    // Return type of module
    std::string_view type() const override;

    /*
     * Processing
     */
    private:
    // Run main processing
    bool process(Dissolve &dissolve, ProcessPool &procPool) override;
};

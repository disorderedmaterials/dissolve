// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "io/export/configuration.h"
#include "module/module.h"

// Export Configuration Module
class ExportConfigurationModule : public Module
{
    public:
    ExportConfigurationModule();
    ~ExportConfigurationModule() override = default;

    /*
     * Definition
     */
    private:
    // Target configuration
    Configuration *targetConfiguration_{nullptr};
    // Whether to tag (suffix) the filename with the current iteration index
    bool tagWithIteration_{false};
    // Filename and format for coordinate export
    CoordinateExportFileFormat coordinatesFormat_;

    /*
     * Processing
     */
    private:
    // Run main processing
    bool process(Dissolve &dissolve, const ProcessPool &procPool) override;
};

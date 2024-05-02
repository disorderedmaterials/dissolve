// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "io/export/coordinates.h"
#include "module/module.h"

// Export Coordinates Module
class ExportCoordinatesModule : public Module
{
    public:
    ExportCoordinatesModule();
    ~ExportCoordinatesModule() override = default;

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
    Module::ExecutionResult process(ModuleContext &moduleContext) override;
};

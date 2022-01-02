// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

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
    bool process(Dissolve &dissolve, ProcessPool &procPool) override;
};

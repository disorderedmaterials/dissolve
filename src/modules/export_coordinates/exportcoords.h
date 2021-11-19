// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

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
    public:
    // Return type of module
    std::string_view type() const override;

    /*
     * Control
     */
    private:
    // Target configuration
    Configuration *targetConfiguration_;
    // Whether to tag (suffix) the filename with the current iteration index
    bool tagWithIteration_{false};

    protected:
    // Perform any necessary initialisation for the Module
    void initialise() override;

    /*
     * Data
     */
    private:
    // Filename and format for coordinate export
    CoordinateExportFileFormat coordinatesFormat_;

    /*
     * Processing
     */
    private:
    // Run main processing
    bool process(Dissolve &dissolve, ProcessPool &procPool) override;
};

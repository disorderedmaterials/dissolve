// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "io/export/coordinates.h"
#include "module/module.h"

// Export Coordinates Module
class ExportCoordinatesModule : public Module
{
    /*
     * Export data in various formats
     */

    public:
    ExportCoordinatesModule();
    ~ExportCoordinatesModule() override = default;

    /*
     * Instances
     */
    public:
    // Create instance of this module
    Module *createInstance() const override;

    /*
     * Definition
     */
    public:
    // Return type of module
    std::string_view type() const override;
    // Return category for module
    std::string_view category() const override;
    // Return brief description of module
    std::string_view brief() const override;
    // Return the number of Configuration targets this Module requires
    int nRequiredTargets() const override;

    /*
     * Control
     */
    private:
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

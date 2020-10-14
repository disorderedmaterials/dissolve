// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#pragma once

#include "io/export/trajectory.h"
#include "module/module.h"

// Forward Declarations
/* none */

// Export Trajectory Module
class ExportTrajectoryModule : public Module
{
    /*
     * Export data in various formats
     */

    public:
    ExportTrajectoryModule();
    ~ExportTrajectoryModule();

    /*
     * Instances
     */
    public:
    // Create instance of this module
    Module *createInstance() const;

    /*
     * Definition
     */
    public:
    // Return type of module
    std::string_view type() const;
    // Return category for module
    std::string_view category() const;
    // Return brief description of module
    std::string_view brief() const;
    // Return the number of Configuration targets this Module requires
    int nRequiredTargets() const;

    /*
     * Initialisation
     */
    protected:
    // Perform any necessary initialisation for the Module
    void initialise();

    /*
     * Data
     */
    private:
    // Filename and format for trajectory export
    TrajectoryExportFileFormat trajectoryFormat_;

    /*
     * Processing
     */
    private:
    // Run main processing
    bool process(Dissolve &dissolve, ProcessPool &procPool);
};

// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "io/fileandformat.h"

// Forward Declarations
class Configuration;

// Trajectory Export Formats
class TrajectoryExportFileFormat : public FileAndFormat
{
    public:
    // Trajectory Export Formats
    enum TrajectoryExportFormat
    {
        XYZTrajectory,
        nTrajectoryExportFormats
    };
    TrajectoryExportFileFormat(std::string_view filename = "", TrajectoryExportFormat format = XYZTrajectory);

    /*
     * Format Access
     */
    private:
    // Return enum options for TrajectoryExportFormat
    static EnumOptions<TrajectoryExportFileFormat::TrajectoryExportFormat> trajectoryExportFormats();

    public:
    // Return number of available formats
    int nFormats() const;
    // Return format keyword for supplied index
    std::string formatKeyword(int id) const;
    // Return description string for supplied index
    std::string formatDescription(int id) const;
    // Return current format as TrajectoryExportFormat
    TrajectoryExportFormat trajectoryFormat() const;

    /*
     * Filename / Basename
     */
    public:
    // Return whether the file must exist
    bool fileMustExist() const { return false; }

    /*
     * Export Functions
     */
    private:
    // Append XYZ frame to trajectory
    bool exportXYZ(LineParser &parser, Configuration *cfg);

    public:
    // Append trajectory using current filename and format
    bool exportData(Configuration *cfg);
};

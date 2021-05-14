// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "io/fileandformat.h"

// Forward Declarations
/* none */

// Trajectory Import Formats
class TrajectoryImportFileFormat : public FileAndFormat
{
    public:
    // Available trajectory formats
    enum TrajectoryImportFormat
    {
        XYZTrajectory,
        nTrajectoryImportFormats
    };
    TrajectoryImportFileFormat(TrajectoryImportFormat format = XYZTrajectory);
    TrajectoryImportFileFormat(std::string_view filename, TrajectoryImportFormat format = XYZTrajectory);
    ~TrajectoryImportFileFormat() override;

    /*
     * Keyword Options
     */
    private:
    // Set up keywords for the format
    void setUpKeywords();

    /*
     * Format Access
     */
    private:
    // Return enum options for TrajectoryImportFileFormat
    static EnumOptions<TrajectoryImportFileFormat::TrajectoryImportFormat> trajectoryImportFormats();

    public:
    // Return number of available formats
    int nFormats() const override;
    // Return format keyword for supplied index
    std::string formatKeyword(int id) const override;
    // Return description string for supplied index
    std::string formatDescription(int id) const override;
    // Return current format as TrajectoryImportFormat
    TrajectoryImportFormat trajectoryFormat() const;

    /*
     * Filename / Basename
     */
    public:
    // Return whether the file must exist
    bool fileMustExist() const override { return true; }
};

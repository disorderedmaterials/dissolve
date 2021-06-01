// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "io/fileandformat.h"

// Forward Declarations
class Configuration;
class ProcessPool;

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

    explicit TrajectoryImportFileFormat(std::string_view filename = "", TrajectoryImportFormat format = XYZTrajectory);
    ~TrajectoryImportFileFormat() override = default;

    /*
     * Formats
     */
    private:
    // Format enum options
    EnumOptions<TrajectoryImportFileFormat::TrajectoryImportFormat> formats_;

    /*
     * Filename / Basename
     */
    public:
    // Return whether the file must exist
    bool fileMustExist() const override { return true; }

    /*
     * Import Functions
     */
    public:
    // Import trajectory using supplied parser and current format
    bool importData(LineParser &parser, Configuration *cfg);
};

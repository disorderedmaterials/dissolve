// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "io/fileAndFormat.h"

// Forward Declarations
class Configuration;

// Trajectory Export Formats
class TrajectoryExportFileFormat : public FileAndFormat
{
    public:
    // Trajectory Export Formats
    enum class TrajectoryExportFormat
    {
        XYZ
    };
    TrajectoryExportFileFormat(std::string_view filename = "", TrajectoryExportFormat format = TrajectoryExportFormat::XYZ);
    ~TrajectoryExportFileFormat() override = default;

    /*
     * Formats
     */
    private:
    // Format enum options
    EnumOptions<TrajectoryExportFileFormat::TrajectoryExportFormat> formats_;

    /*
     * Filename / Basename
     */
    public:
    // Return whether the file must exist
    bool fileMustExist() const override { return false; }

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

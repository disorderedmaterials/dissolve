// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "io/fileAndFormat.h"
#include "math/matrix3.h"

// Forward Declarations
class Configuration;

// Trajectory Import Formats
class TrajectoryImportFileFormat : public FileAndFormat
{
    public:
    // Available trajectory formats
    enum class TrajectoryImportFormat
    {
        DLPOLYFormatted,
        XYZ
    };

    explicit TrajectoryImportFileFormat(std::string_view filename = "",
                                        TrajectoryImportFormat format = TrajectoryImportFormat::XYZ);
    ~TrajectoryImportFileFormat() override = default;

    // Return enum options for TrajectoryImportFileFormat
    static EnumOptions<TrajectoryImportFileFormat::TrajectoryImportFormat> trajectoryImportFileFormats();

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
    private:
    // Import DL_POLY coordinates through specified parser
    bool importDLPOLY(LineParser &parser, std::vector<Vector3> &r, std::optional<Matrix3> &unitCell);

    public:
    // Import trajectory using supplied parser and current format
    bool importData(LineParser &parser, Configuration *cfg, std::optional<Matrix3> &unitCell);
};

EnumOptions<TrajectoryImportFileFormat::TrajectoryImportFormat>
    getEnumOptions(TrajectoryImportFileFormat::TrajectoryImportFormat);
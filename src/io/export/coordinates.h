// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "io/fileAndFormat.h"

// Forward Declarations
class Configuration;

// Coordinate Export Formats
class CoordinateExportFileFormat : public FileAndFormat
{
    public:
    // Available coordinate formats
    enum class CoordinateExportFormat
    {
        XYZ,
        DLPOLY
    };
    CoordinateExportFileFormat(std::string_view filename = "", CoordinateExportFormat format = CoordinateExportFormat::XYZ);
    ~CoordinateExportFileFormat() override = default;

    /*
     * Formats
     */
    private:
    // Format enum options
    EnumOptions<CoordinateExportFileFormat::CoordinateExportFormat> formats_;

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
    // Export Configuration as XYZ
    bool exportXYZ(LineParser &parser, Configuration *cfg);
    // Export Configuration as DL_POLY CONFIG
    bool exportDLPOLY(LineParser &parser, Configuration *cfg);

    public:
    // Export Configuration using current filename and format
    bool exportData(Configuration *cfg);
};

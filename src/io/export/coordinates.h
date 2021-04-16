// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "io/fileandformat.h"

// Forward Declarations
class Configuration;

// Coordinate Export Formats
class CoordinateExportFileFormat : public FileAndFormat
{
    public:
    // Available coordinate formats
    enum CoordinateExportFormat
    {
        XYZCoordinates,
        DLPOLYCoordinates,
        nCoordinateExportFormats
    };
    CoordinateExportFileFormat(std::string_view filename = "", CoordinateExportFormat format = XYZCoordinates);

    /*
     * Format Access
     */
    public:
    // Return enum options for CoordinateExportFormat
    static EnumOptions<CoordinateExportFileFormat::CoordinateExportFormat> coordinateExportFormats();
    // Return number of available formats
    int nFormats() const;
    // Return format keyword for supplied index
    std::string formatKeyword(int id) const;
    // Return description string for supplied index
    std::string formatDescription(int id) const;
    // Return current format as CoordinateExportFormat
    CoordinateExportFormat coordinateFormat() const;

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
    // Export Configuration as XYZ
    bool exportXYZ(LineParser &parser, Configuration *cfg);
    // Export Configuration as DL_POLY CONFIG
    bool exportDLPOLY(LineParser &parser, Configuration *cfg);

    public:
    // Export Configuration using current filename and format
    bool exportData(Configuration *cfg);
};

// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "io/fileandformat.h"
#include "templates/array.h"

// Forward Declarations
class ProcessPool;

// Coordinate Import Formats
class CoordinateImportFileFormat : public FileAndFormat
{
    public:
    // Coordinate Import Formats
    enum CoordinateImportFormat
    {
        DLPOLYCoordinates,
        EPSRCoordinates,
        MoscitoCoordinates,
        XYZCoordinates,
        nCoordinateImportFormats
    };
    CoordinateImportFileFormat(CoordinateImportFormat format = XYZCoordinates);
    CoordinateImportFileFormat(std::string_view filename, CoordinateImportFormat format = XYZCoordinates);
    ~CoordinateImportFileFormat();

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
    // Return enum options for CoordinateImportFormat
    static EnumOptions<CoordinateImportFileFormat::CoordinateImportFormat> coordinateImportFormats();

    public:
    // Return number of available formats
    int nFormats() const;
    // Return format keyword for supplied index
    std::string formatKeyword(int id) const;
    // Return description string for supplied index
    std::string formatDescription(int id) const;
    // Return current format as CoordinateImportFormat
    CoordinateImportFormat coordinateFormat() const;

    /*
     * Filename / Basename
     */
    public:
    // Return whether the file must exist
    bool fileMustExist() const { return true; }

    /*
     * Import Functions
     */
    private:
    // Import DL_POLY coordinates through specified parser
    bool importDLPOLY(LineParser &parser, Array<Vec3<double>> &r);
    // Import EPSR ATO coordinates through specified parser
    bool importEPSR(LineParser &parser, Array<Vec3<double>> &r);
    // Import Moscito coordinates through specified parser
    bool importMoscito(LineParser &parser, Array<Vec3<double>> &r);
    // Import xyz coordinates through specified parser
    bool importXYZ(LineParser &parser, Array<Vec3<double>> &r);

    public:
    // Import coordinates using current filename and format
    bool importData(Array<Vec3<double>> &r, ProcessPool *procPool = nullptr);
    // Import coordinates using supplied parser and current format
    bool importData(LineParser &parser, Array<Vec3<double>> &r);
};

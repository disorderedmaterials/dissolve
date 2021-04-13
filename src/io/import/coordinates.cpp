// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "io/import/coordinates.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"

CoordinateImportFileFormat::CoordinateImportFileFormat(CoordinateImportFileFormat::CoordinateImportFormat format)
    : FileAndFormat(format)
{
    setUpKeywords();
}
CoordinateImportFileFormat::CoordinateImportFileFormat(std::string_view filename,
                                                       CoordinateImportFileFormat::CoordinateImportFormat format)
    : FileAndFormat(filename, format)
{
    setUpKeywords();
}

CoordinateImportFileFormat::~CoordinateImportFileFormat() {}

/*
 * Keyword Options
 */

// Set up keywords for the format
void CoordinateImportFileFormat::setUpKeywords() {}

/*
 * Format Access
 */

// Return enum options for CoordinateImportFormat
EnumOptions<CoordinateImportFileFormat::CoordinateImportFormat> CoordinateImportFileFormat::coordinateImportFormats()
{
    return EnumOptions<CoordinateImportFileFormat::CoordinateImportFormat>(
        "CoordinateImportFileFormat", {{CoordinateImportFileFormat::DLPOLYCoordinates, "dlpoly", "DL_POLY CONFIG"},
                                       {CoordinateImportFileFormat::EPSRCoordinates, "epsr", "EPSR ATO"},
                                       {CoordinateImportFileFormat::MoscitoCoordinates, "moscito", "Moscito structure file"},
                                       {CoordinateImportFileFormat::XYZCoordinates, "xyz", "Simple XYZ"}});
}

// Return number of available formats
int CoordinateImportFileFormat::nFormats() const { return CoordinateImportFileFormat::nCoordinateImportFormats; }

// Return format keyword for supplied index
std::string CoordinateImportFileFormat::formatKeyword(int id) const { return coordinateImportFormats().keywordByIndex(id); }

// Return description string for supplied index
std::string CoordinateImportFileFormat::formatDescription(int id) const
{
    return coordinateImportFormats().descriptionByIndex(id);
}

// Return current format as CoordinateImportFormat
CoordinateImportFileFormat::CoordinateImportFormat CoordinateImportFileFormat::coordinateFormat() const
{
    return (CoordinateImportFileFormat::CoordinateImportFormat)format_;
}

/*
 * Import Functions
 */

// Import coordinates using current filename and format
bool CoordinateImportFileFormat::importData(Array<Vec3<double>> &r, ProcessPool *procPool)
{
    // Open file and check that we're OK to proceed importing from it
    LineParser parser(procPool);
    if ((!parser.openInput(filename_)) || (!parser.isFileGoodForReading()))
        return Messenger::error("Couldn't open file '{}' for loading coordinates data.\n", filename_);

    // Import the data
    auto result = importData(parser, r);

    parser.closeFiles();

    return result;
}

// Import coordinates using supplied parser and current format
bool CoordinateImportFileFormat::importData(LineParser &parser, Array<Vec3<double>> &r)
{
    // Import the data
    auto result = false;
    switch (coordinateFormat())
    {
        case (CoordinateImportFileFormat::DLPOLYCoordinates):
            result = importDLPOLY(parser, r);
            break;
        case (CoordinateImportFileFormat::EPSRCoordinates):
            result = importEPSR(parser, r);
            break;
        case (CoordinateImportFileFormat::MoscitoCoordinates):
            result = importMoscito(parser, r);
            break;
        case (CoordinateImportFileFormat::XYZCoordinates):
            result = importXYZ(parser, r);
            break;
        default:
            Messenger::error("Don't know how to load coordinates in format '{}'.\n", formatKeyword(coordinateFormat()));
    }

    return result;
}

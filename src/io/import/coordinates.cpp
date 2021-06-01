// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "io/import/coordinates.h"
#include "base/lineparser.h"
#include "base/processpool.h"
#include "base/sysfunc.h"
#include "classes/configuration.h"
#include "templates/algorithms.h"

CoordinateImportFileFormat::CoordinateImportFileFormat(CoordinateImportFileFormat::CoordinateImportFormat format)
    : FileAndFormat(formats_)
{
    formats_ = EnumOptions<CoordinateImportFileFormat::CoordinateImportFormat>(
        "CoordinateImportFileFormat",
        {{CoordinateImportFileFormat::DLPOLYCoordinates, "dlpoly", "DL_POLY CONFIG"},
         {CoordinateImportFileFormat::EPSRCoordinates, "epsr", "EPSR ATO"},
         {CoordinateImportFileFormat::MoscitoCoordinates, "moscito", "Moscito structure file"},
         {CoordinateImportFileFormat::XYZCoordinates, "xyz", "Simple XYZ"}},
        format);
    setUpKeywords();
}
CoordinateImportFileFormat::CoordinateImportFileFormat(std::string_view filename,
                                                       CoordinateImportFileFormat::CoordinateImportFormat format)
    : FileAndFormat(formats_, filename)
{
    formats_ = EnumOptions<CoordinateImportFileFormat::CoordinateImportFormat>(
        "CoordinateImportFileFormat",
        {{CoordinateImportFileFormat::DLPOLYCoordinates, "dlpoly", "DL_POLY CONFIG"},
         {CoordinateImportFileFormat::EPSRCoordinates, "epsr", "EPSR ATO"},
         {CoordinateImportFileFormat::MoscitoCoordinates, "moscito", "Moscito structure file"},
         {CoordinateImportFileFormat::XYZCoordinates, "xyz", "Simple XYZ"}},
        format);
    setUpKeywords();
}

CoordinateImportFileFormat::~CoordinateImportFileFormat() = default;

/*
 * Keyword Options
 */

// Set up keywords for the format
void CoordinateImportFileFormat::setUpKeywords() {}

/*
 * Import Functions
 */

// Import coordinates using current filename and format
bool CoordinateImportFileFormat::importData(std::vector<Vec3<double>> &r, ProcessPool *procPool)
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

// Import coordinates direct to configuration using current filename and format
bool CoordinateImportFileFormat::importData(Configuration *cfg, ProcessPool *procPool)
{
    // Open file and check that we're OK to proceed importing from it
    LineParser parser(procPool);
    if ((!parser.openInput(filename_)) || (!parser.isFileGoodForReading()))
        return Messenger::error("Couldn't open file '{}' for loading coordinates data.\n", filename_);

    // Import the data
    std::vector<Vec3<double>> r;
    auto result = importData(parser, cfg);
    parser.closeFiles();

    return result;
}

// Import coordinates using supplied parser and current format
bool CoordinateImportFileFormat::importData(LineParser &parser, std::vector<Vec3<double>> &r)
{
    // Import the data
    auto result = false;
    switch (formats_.enumeration())
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
            throw(std::runtime_error(
                fmt::format("Coordinate format '{}' import has not been implemented.\n", formats_.keyword())));
    }

    return result;
}

// Import coordinates direct to configuration using supplied parser and current format
bool CoordinateImportFileFormat::importData(LineParser &parser, Configuration *cfg)
{
    // Import the data
    std::vector<Vec3<double>> r;
    auto result = importData(parser, r);
    if (!result)
        return Messenger::error("Couldn't import configuration coordinates from file.\n");

    // Temporary array now contains some number of atoms - does it match the number in the configuration's molecules?
    if (cfg->nAtoms() != r.size())
        return Messenger::error(
            "Number of atoms read from initial coordinates file ({}) does not match that in Configuration ({}).\n", r.size(),
            cfg->nAtoms());

    // All good, so copy atom coordinates over into our array
    for (auto &&[i, ri] : zip(cfg->atoms(), r))
        i->setCoordinates(ri);

    return true;
}

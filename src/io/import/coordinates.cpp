// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "io/import/coordinates.h"
#include "base/lineParser.h"
#include "base/processPool.h"
#include "base/sysFunc.h"
#include "classes/configuration.h"
#include "templates/algorithms.h"

CoordinateImportFileFormat::CoordinateImportFileFormat(std::string_view filename,
                                                       CoordinateImportFileFormat::CoordinateImportFormat format)
    : FileAndFormat(formats_, filename, (int)format)
{
    formats_ = EnumOptions<CoordinateImportFileFormat::CoordinateImportFormat>(
        "CoordinateImportFileFormat", {{CoordinateImportFormat::DLPOLY, "dlpoly", "DL_POLY CONFIG"},
                                       {CoordinateImportFormat::EPSR, "epsr", "EPSR ATO"},
                                       {CoordinateImportFormat::Moscito, "moscito", "Moscito structure file"},
                                       {CoordinateImportFormat::XYZ, "xyz", "Simple XYZ"}});

    setUpKeywords();
}

/*
 * Keyword Options
 */

// Set up keywords for the format
void CoordinateImportFileFormat::setUpKeywords() {}

/*
 * Import Functions
 */

// Import coordinates using current filename and format
bool CoordinateImportFileFormat::importData(std::vector<Vec3<double>> &r, const ProcessPool *procPool)
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
bool CoordinateImportFileFormat::importData(Configuration *cfg, const ProcessPool *procPool)
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
    // Check the format
    if (!formatIndex_)
        return Messenger::error("No format set for ConfigurationImportFileFormat so can't import.\n");

    // Import the data
    auto result = false;
    switch (formats_.enumerationByIndex(*formatIndex_))
    {
        case (CoordinateImportFormat::DLPOLY):
            result = importDLPOLY(parser, r);
            break;
        case (CoordinateImportFormat::EPSR):
            result = importEPSR(parser, r);
            break;
        case (CoordinateImportFormat::Moscito):
            result = importMoscito(parser, r);
            break;
        case (CoordinateImportFormat::XYZ):
            result = importXYZ(parser, r);
            break;
        default:
            throw(std::runtime_error(fmt::format("Coordinate format '{}' import has not been implemented.\n",
                                                 formats_.keywordByIndex(*formatIndex_))));
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
        i.setCoordinates(ri);

    return true;
}

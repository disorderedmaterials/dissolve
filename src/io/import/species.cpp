// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "io/import/species.h"
#include "base/lineParser.h"
#include "base/processPool.h"
#include "base/sysFunc.h"
#include "classes/species.h"
#include "templates/algorithms.h"

SpeciesImportFileFormat::SpeciesImportFileFormat(std::string_view filename, SpeciesImportFileFormat::SpeciesImportFormat format)
    : FileAndFormat(formats_, filename, (int)format)
{
    formats_ = EnumOptions<SpeciesImportFileFormat::SpeciesImportFormat>("SpeciesImportFileFormat",
                                                                         {{SpeciesImportFormat::XYZ, "xyz", "Simple XYZ"}});

    setUpKeywords();
}

/*
 * Keyword Options
 */

// Set up keywords for the format
void SpeciesImportFileFormat::setUpKeywords() {}

/*
 * Import Functions
 */

// Import coordinates direct to configuration using current filename and format
bool SpeciesImportFileFormat::importData(Species *sp, const ProcessPool *procPool)
{
    // Open file and check that we're OK to proceed importing from it
    LineParser parser(procPool);
    if ((!parser.openInput(filename_)) || (!parser.isFileGoodForReading()))
        return Messenger::error("Couldn't open file '{}' for loading coordinates data.\n", filename_);

    // Import the data
    auto result = false;
    try
    {
        result = importData(parser, sp);
        parser.closeFiles();
    }
    catch (...)
    {
        return Messenger::error("Failed to import species data ({}) from file '{}'.\n", formats_.keywordByIndex(*formatIndex_),
                                filename_);
    }

    return result;
}

// Import coordinates direct to configuration using supplied parser and current format
bool SpeciesImportFileFormat::importData(LineParser &parser, Species *sp)
{
    // Check the format
    if (!formatIndex_)
        return Messenger::error("No format set for SpeciesImportFileFormat so can't import.\n");

    // Import the data
    auto result = false;
    switch (formats_.enumerationByIndex(*formatIndex_))
    {
        case (SpeciesImportFormat::XYZ):
            result = importXYZ(parser, sp);
            break;
        default:
            throw(std::runtime_error(
                fmt::format("Species format '{}' import has not been implemented.\n", formats_.keywordByIndex(*formatIndex_))));
    }

    return result;
}

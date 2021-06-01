// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "io/import/forces.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "keywords/double.h"

ForceImportFileFormat::ForceImportFileFormat(ForceImportFileFormat::ForceImportFormat format) : FileAndFormat(formats_)
{
    formats_ = EnumOptions<ForceImportFileFormat::ForceImportFormat>(
        "ForceImportFileFormat",
        {{ForceImportFileFormat::DLPOLYForces, "dlpoly", "DL_POLY Config File Forces"},
         {ForceImportFileFormat::MoscitoForces, "moscito", "Moscito Structure File Forces"},
         {ForceImportFileFormat::SimpleForces, "simple", "Simple Free-Formatted Forces"}},
        format);
    setUpKeywords();
}
ForceImportFileFormat::ForceImportFileFormat(std::string_view filename, ForceImportFileFormat::ForceImportFormat format)
    : FileAndFormat(formats_, filename)
{
    formats_ = EnumOptions<ForceImportFileFormat::ForceImportFormat>(
        "ForceImportFileFormat",
        {{ForceImportFileFormat::DLPOLYForces, "dlpoly", "DL_POLY Config File Forces"},
         {ForceImportFileFormat::MoscitoForces, "moscito", "Moscito Structure File Forces"},
         {ForceImportFileFormat::SimpleForces, "simple", "Simple Free-Formatted Forces"}},
        format);
    setUpKeywords();
}

ForceImportFileFormat::~ForceImportFileFormat() = default;

/*
 * Keyword Options
 */

// Set up keywords for the format
void ForceImportFileFormat::setUpKeywords()
{
    keywords_.add("Conversion", new DoubleKeyword(1.0), "Factor", "Factor to multiply forces by (for unit conversion etc.)");
}

/*
 * Import Functions
 */

// Read forces using current filename and format
bool ForceImportFileFormat::importData(std::vector<Vec3<double>> &f, ProcessPool *procPool)
{
    // Open file and check that we're OK to proceed importing from it
    LineParser parser(procPool);
    if ((!parser.openInput(filename_)) || (!parser.isFileGoodForReading()))
        return Messenger::error("Couldn't open file '{}' for loading forces data.\n", filename_);

    // Import the data
    auto result = importData(parser, f);

    parser.closeFiles();

    return result;
}

// Import forces using supplied parser and current format
bool ForceImportFileFormat::importData(LineParser &parser, std::vector<Vec3<double>> &f)
{
    // Import the data
    auto result = false;
    switch (formats_.enumeration())
    {
        case (ForceImportFileFormat::DLPOLYForces):
            result = importDLPOLY(parser, f);
            break;
        case (ForceImportFileFormat::MoscitoForces):
            result = importMoscito(parser, f);
            break;
        case (ForceImportFileFormat::SimpleForces):
            result = importSimple(parser, f);
            break;
        default:
            throw(std::runtime_error(fmt::format("Force format '{}' import has not been implemented.\n", formats_.keyword())));
    }

    // Apply factor to data
    auto factor = keywords_.asDouble("Factor");
    std::transform(f.begin(), f.end(), f.begin(), [factor](auto &value) { return value * factor; });

    return result;
}

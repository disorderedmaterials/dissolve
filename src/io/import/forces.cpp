// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "io/import/forces.h"
#include "base/lineParser.h"
#include "base/sysFunc.h"
#include "keywords/double.h"

ForceImportFileFormat::ForceImportFileFormat(std::string_view filename, ForceImportFileFormat::ForceImportFormat format)
    : FileAndFormat(formats_, filename, (int)format)
{
    formats_ = EnumOptions<ForceImportFileFormat::ForceImportFormat>(
        "ForceImportFileFormat", {{ForceImportFormat::DLPOLY, "dlpoly", "DL_POLY Config File Forces"},
                                  {ForceImportFormat::Moscito, "moscito", "Moscito Structure File Forces"},
                                  {ForceImportFormat::Simple, "simple", "Simple Free-Formatted Forces"}});

    setUpKeywords();
}

/*
 * Keyword Options
 */

// Set up keywords for the format
void ForceImportFileFormat::setUpKeywords()
{
    keywords_.setOrganisation("Options", "Conversion");
    keywords_.add<DoubleKeyword>("Factor", "Factor to multiply forces by (for unit conversion etc.)", factor_);
}

/*
 * Import Functions
 */

// Read forces using current filename and format
bool ForceImportFileFormat::importData(std::vector<Vec3<double>> &f, const ProcessPool *procPool)
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
    // Check the format
    if (!formatIndex_)
        return Messenger::error("No format set for ForceImportFileFormat so can't import.\n");

    // Import the data
    auto result = false;
    switch (formats_.enumerationByIndex(*formatIndex_))
    {
        case (ForceImportFormat::DLPOLY):
            result = importDLPOLY(parser, f);
            break;
        case (ForceImportFormat::Moscito):
            result = importMoscito(parser, f);
            break;
        case (ForceImportFormat::Simple):
            result = importSimple(parser, f);
            break;
        default:
            throw(std::runtime_error(
                fmt::format("Force format '{}' import has not been implemented.\n", formats_.keywordByIndex(*formatIndex_))));
    }

    // Apply factor to data
    std::transform(f.begin(), f.end(), f.begin(), [&](auto &value) { return value * factor_; });

    return result;
}

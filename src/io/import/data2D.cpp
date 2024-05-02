// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "io/import/data2D.h"
#include "base/lineParser.h"
#include "base/sysFunc.h"
#include "keywords/vec3Double.h"

Data2DImportFileFormat::Data2DImportFileFormat(std::string_view filename, Data2DImportFileFormat::Data2DImportFormat format)
    : FileAndFormat(formats_, filename, (int)format)
{
    formats_ = EnumOptions<Data2DImportFileFormat::Data2DImportFormat>(
        "Data2DImportFileFormat", {{Data2DImportFormat::Cartesian, "cartesian", "Cartesian X,Y,f(X,Y) data"}});

    setUpKeywords();
}

/*
 * Keyword Options
 */

// Set up keywords for the format
void Data2DImportFileFormat::setUpKeywords()
{
    keywords_.setOrganisation("Options", "Ranges");
    keywords_.add<Vec3DoubleKeyword>("XAxis", "Min, max, and delta to assume for x axis", xAxis_,
                                     Vec3Labels::MinMaxDeltaLabels);
    keywords_.add<Vec3DoubleKeyword>("YAxis", "Min, max, and delta to assume for y axis", yAxis_,
                                     Vec3Labels::MinMaxDeltaLabels);
}

/*
 * Import Functions
 */

// Import Data2D using current filename and format
bool Data2DImportFileFormat::importData(Data2D &data, const ProcessPool *procPool)
{
    // Open file and check that we're OK to proceed importing from it
    LineParser parser(procPool);
    if ((!parser.openInput(filename_)) || (!parser.isFileGoodForReading()))
        return Messenger::error("Couldn't open file '{}' for loading Data2D data.\n", filename_);

    // Import the data
    auto result = importData(parser, data);

    parser.closeFiles();

    return result;
}

// Import Data2D using supplied parser and current format
bool Data2DImportFileFormat::importData(LineParser &parser, Data2D &data)
{
    // Check the format
    if (!formatIndex_)
        return Messenger::error("No format set for Data2DImportFileFormat so can't import.\n");

    // Import the data
    auto result = false;
    switch (formats_.enumerationByIndex(*formatIndex_))
    {
        case (Data2DImportFormat::Cartesian):
            result = importCartesian(parser, data);
            break;
        default:
            throw(std::runtime_error(
                fmt::format("Data2D format '{}' import has not been implemented.\n", formats_.keywordByIndex(*formatIndex_))));
    }

    return result;
}

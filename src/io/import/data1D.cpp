// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "io/import/data1D.h"
#include "base/lineParser.h"
#include "base/sysFunc.h"
#include "keywords/integer.h"
#include "keywords/optionalDouble.h"
#include "math/data1D.h"
#include "math/filters.h"

Data1DImportFileFormat::Data1DImportFileFormat(std::string_view filename, Data1DImportFileFormat::Data1DImportFormat format,
                                               int xColumn, int yColumn, int errorColumn)
    : FileAndFormat(formats_, filename, (int)format), xColumn_(xColumn), yColumn_(yColumn), errorColumn_(errorColumn)
{
    formats_ = EnumOptions<Data1DImportFileFormat::Data1DImportFormat>(
        "Data1DImportFileFormat", {{Data1DImportFormat::XY, "xy", "Simple XY data (x = bin centres)"},
                                   {Data1DImportFormat::Histogram, "histogram", "Histogrammed Data (x = bin left-boundaries)"},
                                   {Data1DImportFormat::GudrunMint, "mint", "Gudrun output (mint01)"}});

    setUpKeywords();
}

/*
 * Keyword Options
 */

// Set up keywords for the format
void Data1DImportFileFormat::setUpKeywords()
{
    keywords_.setOrganisation("Options", "Columns");
    keywords_.add<IntegerKeyword>("X", "Column index to use for x values", xColumn_, 1);
    keywords_.add<IntegerKeyword>("Y", "Column index to use for y values", yColumn_, 1);
    keywords_.add<IntegerKeyword>("Error", "Column index to use for error values", errorColumn_, 0);

    keywords_.setOrganisation("Options", "Manipulation");
    keywords_.add<OptionalDoubleKeyword>("RemoveAverage", "X axis value from which to form average value to subtract from data",
                                         removeAverageFromX_, 0.0, std::nullopt, 0.1, "Off");
    keywords_.add<OptionalDoubleKeyword>("XMin", "Minimum X value to allow when reading in the data", xMin_, 0.0, std::nullopt,
                                         0.1, "No Minimum");
    keywords_.add<OptionalDoubleKeyword>("XMax", "Maximum X value to allow when reading in the data", xMax_, 0.0, std::nullopt,
                                         0.1, "No Maximum");
    keywords_.add<IntegerKeyword>("RemovePoints", "Remove a number of points from the start of the data", nPointsToRemove_, 0);
}

/*
 * Import Functions
 */

// Import Data1D using current filename and format
bool Data1DImportFileFormat::importData(Data1D &data, const ProcessPool *procPool)
{
    // Open file and check that we're OK to proceed importing from it
    LineParser parser(procPool);
    if ((!parser.openInput(filename_)) || (!parser.isFileGoodForReading()))
        return Messenger::error("Couldn't open file '{}' for loading Data1D data.\n", filename_);

    // Import the data
    auto result = importData(parser, data);

    parser.closeFiles();

    // Validity check on number of points in loaded file
    if (result && data.nValues() == 0)
        return Messenger::error("File '{}' contains no data.\n", filename_);

    return result;
}

// Import Data1D using supplied parser and current format
bool Data1DImportFileFormat::importData(LineParser &parser, Data1D &data)
{
    // Check the format
    if (!formatIndex_)
        return Messenger::error("No format set for Data1DImportFileFormat so can't import.\n");

    // Import the data
    auto result = false;
    switch (formats_.enumerationByIndex(*formatIndex_))
    {
        case (Data1DImportFormat::XY):
            result = importXY(parser, data);
            break;
        case (Data1DImportFormat::Histogram):
            result = importHistogram(parser, data);
            break;
        case (Data1DImportFormat::GudrunMint):
            result = importGudrunMint(parser, data);
            break;
        default:
            throw(std::runtime_error(
                fmt::format("Data1D format '{}' import has not been implemented.\n", formats_.keywordByIndex(*formatIndex_))));
    }

    // If we failed, may as well return now
    if (!result)
        return false;

    // Handle any additional options
    // -- Remove points from the start of the data?
    for (auto n = 0; n < nPointsToRemove_; ++n)
        data.removeFirstPoint();
    // -- Trim range?
    if (xMin_ || xMax_)
    {
        auto xMin = xMin_.value_or(data.xAxis().front() - 1.0);
        auto xMax = xMax_.value_or(data.xAxis().back() + 1.0);
        Filters::trim(data, xMin, xMax);
    }
    // -- Subtract average level from data?
    if (removeAverageFromX_)
    {
        double level = Filters::subtractAverage(data, removeAverageFromX_.value());
        Messenger::print("Removed average level of {} from data, forming average over x >= {}.\n", level,
                         removeAverageFromX_.value());
    }

    return result;
}

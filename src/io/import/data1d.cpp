// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "io/import/data1d.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "keywords/double.h"
#include "keywords/integer.h"
#include "math/data1d.h"
#include "math/filters.h"

Data1DImportFileFormat::Data1DImportFileFormat(std::string_view filename, Data1DImportFileFormat::Data1DImportFormat format)
    : FileAndFormat(formats_, filename)
{
    formats_ = EnumOptions<Data1DImportFileFormat::Data1DImportFormat>(
        "Data1DImportFileFormat",
        {{Data1DImportFormat::XY, "xy", "Simple XY data (x = bin centres)"},
         {Data1DImportFormat::Histogram, "histogram", "Histogrammed Data (x = bin left-boundaries)"},
         {Data1DImportFormat::GudrunMint, "mint", "Gudrun output (mint01)"}},
        format);
    setUpKeywords();
}

/*
 * Keyword Options
 */

// Set up keywords for the format
void Data1DImportFileFormat::setUpKeywords()
{
    keywords_.add<IntegerKeyword>("Columns", "X", "Column index to use for x values", xColumn_, 1);
    keywords_.add<IntegerKeyword>("Columns", "Y", "Column index to use for y values", yColumn_, 1);
    keywords_.add<IntegerKeyword>("Columns", "Error", "Column index to use for error values", errorColumn_, 0);
    keywords_.add<DoubleKeyword>("Manipulations", "RemoveAverage",
                                 "X axis value from which to form average value to subtract from data (-1 for no subtraction)",
                                 removeAverageFromX_, -1.0);
    keywords_.add<DoubleKeyword>("Manipulations", "XMin", "Set the minimum X value to allow when reading in the data", xMin_);
    keywords_.add<DoubleKeyword>("Manipulations", "XMax", "Set the maximum X value to allow when reading in the data", xMax_);
    keywords_.add<IntegerKeyword>("Manipulations", "RemovePoints", "Remove a number of points from the start of the data",
                                  nPointsToRemove_, 0);
}

/*
 * Import Functions
 */

// Import Data1D using current filename and format
bool Data1DImportFileFormat::importData(Data1D &data, ProcessPool *procPool)
{
    // Open file and check that we're OK to proceed importing from it
    LineParser parser(procPool);
    if ((!parser.openInput(filename_)) || (!parser.isFileGoodForReading()))
        return Messenger::error("Couldn't open file '{}' for loading Data1D data.\n", filename_);

    // Import the data
    auto result = importData(parser, data);

    parser.closeFiles();

    return result;
}

// Import Data1D using supplied parser and current format
bool Data1DImportFileFormat::importData(LineParser &parser, Data1D &data)
{
    // Import the data
    auto result = false;
    switch (formats_.enumeration())
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
            throw(std::runtime_error(fmt::format("Data1D format '{}' import has not been implemented.\n", formats_.keyword())));
    }

    // If we failed, may as well return now
    if (!result)
        return false;

    // Handle any additional options
    // -- Remove points from the start of the data?
    for (auto n = 0; n < nPointsToRemove_; ++n)
        data.removeFirstPoint();
    // -- Trim range?
    if (keywords_.hasBeenSet("XMin") || keywords_.hasBeenSet("XMax"))
    {
        auto xMin = keywords_.hasBeenSet("XMin") ? xMin_ : data.xAxis().front() - 1.0;
        auto xMax = keywords_.hasBeenSet("XMax") ? xMax_ : data.xAxis().back() + 1.0;
        Filters::trim(data, xMin, xMax);
    }
    // -- Subtract average level from data?
    if (removeAverageFromX_ > 0.0)
    {
        double level = Filters::subtractAverage(data, removeAverageFromX_);
        Messenger::print("Removed average level of {} from data, forming average over x >= {}.\n", level, removeAverageFromX_);
    }

    return result;
}

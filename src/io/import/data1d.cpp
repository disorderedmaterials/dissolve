/*
    *** Import - Data1D
    *** src/io/import/data1d.cpp
    Copyright T. Youngs 2012-2020

    This file is part of Dissolve.

    Dissolve is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Dissolve is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "io/import/data1d.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "math/filters.h"

Data1DImportFileFormat::Data1DImportFileFormat(Data1DImportFileFormat::Data1DImportFormat format) : FileAndFormat(format)
{
    setUpKeywords();
}
Data1DImportFileFormat::Data1DImportFileFormat(std::string_view filename, Data1DImportFileFormat::Data1DImportFormat format)
    : FileAndFormat(filename, format)
{
    setUpKeywords();
}

Data1DImportFileFormat::~Data1DImportFileFormat() {}

/*
 * Keyword Options
 */

// Set up keywords for the format
void Data1DImportFileFormat::setUpKeywords()
{
    keywords_.add("Columns", new IntegerKeyword(1, 1), "X", "Column index to use for x values");
    keywords_.add("Columns", new IntegerKeyword(2, 1), "Y", "Column index to use for y values");
    keywords_.add("Columns", new IntegerKeyword(0, 0), "Error", "Column index to use for error values");
    keywords_.add("Manipulations", new DoubleKeyword(-1.0, -1.0), "RemoveAverage",
                  "X axis value from which to form average value to subtract from data (-1 for no subtraction)");
}

/*
 * Format Access
 */

// Return enum options for Data1DImportFormat
EnumOptions<Data1DImportFileFormat::Data1DImportFormat> &Data1DImportFileFormat::data1DImportFormats()
{
    static EnumOptionsList Data1DImportFormats =
        EnumOptionsList() << EnumOption(Data1DImportFileFormat::XYData1D, "xy", "Simple XY data (x = bin centres)")
                          << EnumOption(Data1DImportFileFormat::HistogramData1D, "histogram",
                                        "Histogrammed Data (x = bin left-boundaries)")
                          << EnumOption(Data1DImportFileFormat::GudrunMintData1D, "mint", "Gudrun output (mint01)");

    static EnumOptions<Data1DImportFileFormat::Data1DImportFormat> options("Data1DImportFileFormat", Data1DImportFormats);

    return options;
}

// Return number of available formats
int Data1DImportFileFormat::nFormats() const { return Data1DImportFileFormat::nData1DImportFormats; }

// Return format keyword for supplied index
std::string_view Data1DImportFileFormat::formatKeyword(int id) const { return data1DImportFormats().keywordByIndex(id); }

// Return description string for supplied index
std::string_view Data1DImportFileFormat::formatDescription(int id) const
{
    return data1DImportFormats().descriptionByIndex(id);
}

// Return current format as Data1DImportFormat
Data1DImportFileFormat::Data1DImportFormat Data1DImportFileFormat::data1DFormat() const
{
    return (Data1DImportFileFormat::Data1DImportFormat)format_;
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
    switch (data1DFormat())
    {
        case (Data1DImportFileFormat::XYData1D):
            result = importXY(parser, data);
            break;
        case (Data1DImportFileFormat::HistogramData1D):
            result = importHistogram(parser, data);
            break;
        case (Data1DImportFileFormat::GudrunMintData1D):
            result = importGudrunMint(parser, data);
            break;
        default:
            Messenger::error("Don't know how to load Data1D of format '{}'.\n", formatKeyword(data1DFormat()));
    }

    // If we failed, may as well return now
    if (!result)
        return false;

    // Handle any additional options
    // --Subtract average level from data?
    const auto removeAverage = keywords_.asDouble("RemoveAverage");
    if (removeAverage > 0.0)
    {
        double level = Filters::subtractAverage(data, removeAverage);
        Messenger::print("Removed average level of {} from data, forming average over x >= {}.\n", level, removeAverage);
    }

    return result;
}

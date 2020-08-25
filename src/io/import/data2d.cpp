/*
    *** Import - Data2D
    *** src/io/import/data2d.cpp
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

#include "io/import/data2d.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "keywords/types.h"

Data2DImportFileFormat::Data2DImportFileFormat(Data2DImportFileFormat::Data2DImportFormat format) : FileAndFormat(format)
{
    setUpKeywords();
}
Data2DImportFileFormat::Data2DImportFileFormat(std::string_view filename, Data2DImportFileFormat::Data2DImportFormat format)
    : FileAndFormat(filename, format)
{
    setUpKeywords();
}

Data2DImportFileFormat::~Data2DImportFileFormat() {}

/*
 * Keyword Options
 */

// Set up keywords for the format
void Data2DImportFileFormat::setUpKeywords()
{
    keywords_.add("Ranges", new Vec3DoubleKeyword(Vec3<double>(0.0, 0.0, 0.0)), "XAxis",
                  "Min, max, and delta to assume for x axis");
    keywords_.add("Ranges", new Vec3DoubleKeyword(Vec3<double>(0.0, 0.0, 0.0)), "YAxis",
                  "Min, max, and delta to assume for y axis");
}

/*
 * Format Access
 */

// Return enum options for Data2DImportFormat
EnumOptions<Data2DImportFileFormat::Data2DImportFormat> &Data2DImportFileFormat::data2DImportFormats()
{
    static EnumOptionsList Data2DImportFormats =
        EnumOptionsList() << EnumOption(Data2DImportFileFormat::CartesianData2D, "cartesian", "Cartesian X,Y,f(X,Y) data");

    static EnumOptions<Data2DImportFileFormat::Data2DImportFormat> options("Data2DImportFileFormat", Data2DImportFormats);

    return options;
}

// Return number of available formats
int Data2DImportFileFormat::nFormats() const { return Data2DImportFileFormat::nData2DImportFormats; }

// Return format keyword for supplied index
std::string_view Data2DImportFileFormat::formatKeyword(int id) const { return data2DImportFormats().keywordByIndex(id); }

// Return description string for supplied index
std::string_view Data2DImportFileFormat::formatDescription(int id) const
{
    return data2DImportFormats().descriptionByIndex(id);
}

// Return current format as Data2DImportFormat
Data2DImportFileFormat::Data2DImportFormat Data2DImportFileFormat::data2DFormat() const
{
    return (Data2DImportFileFormat::Data2DImportFormat)format_;
}

/*
 * Import Functions
 */

// Import Data2D using current filename and format
bool Data2DImportFileFormat::importData(Data2D &data, ProcessPool *procPool)
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
    // Import the data
    auto result = false;
    switch (data2DFormat())
    {
        case (Data2DImportFileFormat::CartesianData2D):
            result = importCartesian(parser, data);
            break;
        default:
            Messenger::error("Don't know how to load Data2D of format '{}'.\n", formatKeyword(data2DFormat()));
    }

    return result;
}

// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "io/import/data2d.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "keywords/types.h"

Data2DImportFileFormat::Data2DImportFileFormat(std::string_view filename, Data2DImportFileFormat::Data2DImportFormat format)
    : FileAndFormat(formats_, filename)
{
    formats_ = EnumOptions<Data2DImportFileFormat::Data2DImportFormat>(
        "Data2DImportFileFormat", {{Data2DImportFormat::Cartesian, "cartesian", "Cartesian X,Y,f(X,Y) data"}}, format);
    setUpKeywords();
}

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
    switch (formats_.enumeration())
    {
        case (Data2DImportFormat::Cartesian):
            result = importCartesian(parser, data);
            break;
        default:
            throw(std::runtime_error(fmt::format("Data2D format '{}' import has not been implemented.\n", formats_.keyword())));
    }

    return result;
}

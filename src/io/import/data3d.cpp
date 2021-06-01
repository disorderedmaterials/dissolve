// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "io/import/data3d.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"

Data3DImportFileFormat::Data3DImportFileFormat(std::string_view filename, Data3DImportFileFormat::Data3DImportFormat format)
    : FileAndFormat(formats_, filename)
{
    formats_ = EnumOptions<Data3DImportFileFormat::Data3DImportFormat>(
        "Data3DImportFileFormat", {{Data3DImportFileFormat::CartesianData3D, "cartesian", "Cartesian X,Y,Z,f(x,y,z) data"}},
        format);
    setUpKeywords();
}

/*
 * Keyword Options
 */

// Set up keywords for the format
void Data3DImportFileFormat::setUpKeywords() {}

/*
 * Import Functions
 */

// Import Data3D using current filename and format
bool Data3DImportFileFormat::importData(Data3D &data, ProcessPool *procPool)
{
    // Open file and check that we're OK to proceed importing from it
    LineParser parser(procPool);
    if ((!parser.openInput(filename_)) || (!parser.isFileGoodForReading()))
        return Messenger::error("Couldn't open file '{}' for loading Data3D data.\n", filename_);

    // Import the data
    auto result = importData(parser, data);

    parser.closeFiles();

    return result;
}

// Import Data3D using supplied parser and current format
bool Data3DImportFileFormat::importData(LineParser &parser, Data3D &data)
{
    // Import the data
    auto result = false;
    switch (formats_.enumeration())
    {
        default:
            throw(std::runtime_error(fmt::format("Data3D format '{}' import has not been implemented.\n", formats_.keyword())));
    }

    return result;
}

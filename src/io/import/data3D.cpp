// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "io/import/data3D.h"
#include "base/lineParser.h"
#include "base/sysFunc.h"

Data3DImportFileFormat::Data3DImportFileFormat(std::string_view filename, Data3DImportFileFormat::Data3DImportFormat format)
    : DataImportFileFormat(formats_, filename, (int)format)
{
    formats_ = EnumOptions<Data3DImportFileFormat::Data3DImportFormat>(
        "Data3DImportFileFormat", {{Data3DImportFormat::PDens, "pdens", "dlputils-style pdens format"}});

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
bool Data3DImportFileFormat::importData(Data3D &data, const ProcessPool *procPool)
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
    // Check the format
    if (!formatIndex_)
        return Messenger::error("No format set for Data3DImportFileFormat so can't import.\n");

    // Import the data
    auto result = false;
    switch (formats_.enumerationByIndex(*formatIndex_))
    {
        case (Data3DImportFormat::PDens):
            result = importPDens(parser, data);
            break;
        default:
            throw(std::runtime_error(
                fmt::format("Data3D format '{}' import has not been implemented.\n", formats_.keywordByIndex(*formatIndex_))));
    }

    return result;
}

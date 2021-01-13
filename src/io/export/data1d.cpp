// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "io/export/data1d.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "math/data1d.h"
#include "templates/algorithms.h"

Data1DExportFileFormat::Data1DExportFileFormat(std::string_view filename, Data1DExportFormat format)
    : FileAndFormat(filename, format)
{
}

/*
 * Format Access
 */

// Return enum options for Data1DExportFormat
EnumOptions<Data1DExportFileFormat::Data1DExportFormat> &Data1DExportFileFormat::data1DExportFormats()
{
    static EnumOptionsList Data1DExportFormats =
        EnumOptionsList() << EnumOption(Data1DExportFileFormat::XYData1D, "xy", "Simple XY data (x = bin centres)");

    static EnumOptions<Data1DExportFileFormat::Data1DExportFormat> options("Data1DExportFileFormat", Data1DExportFormats);

    return options;
}

// Return number of available formats
int Data1DExportFileFormat::nFormats() const { return Data1DExportFileFormat::nData1DExportFormats; }

// Return format keyword for supplied index
std::string_view Data1DExportFileFormat::formatKeyword(int id) const { return data1DExportFormats().keywordByIndex(id); }

// Return description string for supplied index
std::string_view Data1DExportFileFormat::formatDescription(int id) const
{
    return data1DExportFormats().descriptionByIndex(id);
}

// Return current format as CoordinateExportFormat
Data1DExportFileFormat::Data1DExportFormat Data1DExportFileFormat::data1DFormat() const
{
    return (Data1DExportFileFormat::Data1DExportFormat)format_;
}

/*
 * Export Functions
 */

// Export Data1D as simple XY (or XYE) data
bool Data1DExportFileFormat::exportXY(LineParser &parser, const Data1D &data)
{
    if (data.valuesHaveErrors())
    {
        for (auto &&[x, value, error] : zip(data.xAxis(), data.values(), data.errors()))
            if (!parser.writeLineF("{:16.10e}  {:16.10e}  {:16.10e}\n", x, value, error))
                return false;
    }
    else
        for (auto &&[x, value] : zip(data.xAxis(), data.values()))
            if (!parser.writeLineF("{:16.10e}  {:16.10e}\n", x, value))
                return false;

    return true;
}

// Export Data1D using current filename and format
bool Data1DExportFileFormat::exportData(const Data1D &data)
{
    // Open the file
    LineParser parser;
    if (!parser.openOutput(filename_))
    {
        parser.closeFiles();
        return false;
    }

    // Write data
    auto result = false;
    if (data1DFormat() == Data1DExportFileFormat::XYData1D)
        result = exportXY(parser, data);
    else
    {
        Messenger::error("Unrecognised Data1D format.\nKnown formats are:\n");
        printAvailableFormats();
    }

    return result;
}

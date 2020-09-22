// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#include "io/export/data2d.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "math/data2d.h"

Data2DExportFileFormat::Data2DExportFileFormat(std::string_view filename, Data2DExportFormat format)
    : FileAndFormat(filename, format)
{
}

/*
 * Format Access
 */

// Return enum options for Data2DExportFormat
EnumOptions<Data2DExportFileFormat::Data2DExportFormat> &Data2DExportFileFormat::data2DExportFormats()
{
    static EnumOptionsList Data2DExportFormats =
        EnumOptionsList() << EnumOption(Data2DExportFileFormat::BlockData2D, "block", "Block Data")
                          << EnumOption(Data2DExportFileFormat::CartesianData2D, "cartesian", "Cartesian (x,y,value) Data");

    static EnumOptions<Data2DExportFileFormat::Data2DExportFormat> options("Data2DExportFileFormat", Data2DExportFormats);

    return options;
}

// Return number of available formats
int Data2DExportFileFormat::nFormats() const { return Data2DExportFileFormat::nData2DExportFormats; }

// Return format keyword for supplied index
std::string_view Data2DExportFileFormat::formatKeyword(int id) const { return data2DExportFormats().keywordByIndex(id); }

// Return description string for supplied index
std::string_view Data2DExportFileFormat::formatDescription(int id) const
{
    return data2DExportFormats().descriptionByIndex(id);
}

// Return current format as CoordinateExportFormat
Data2DExportFileFormat::Data2DExportFormat Data2DExportFileFormat::data2DFormat() const
{
    return (Data2DExportFileFormat::Data2DExportFormat)format_;
}

/*
 * Export Functions
 */

// Export Data2D as simple block data
bool Data2DExportFileFormat::exportBlock(LineParser &parser, const Data2D &data)
{
    // Export header comment
    if (!parser.writeLineF("# {} blocks (nX) of {} points (nY).\n", data.constXAxis().nItems(), data.constYAxis().nItems()))
        return false;

    // Export datapoints, separating each block of a specific x value with a single blank line
    const Array2D<double> &values = data.constValues2D();
    for (int x = 0; x < values.nRows(); ++x)
    {
        for (int y = 0; y < values.nColumns(); ++y)
            if (!parser.writeLineF("{:15.9f}\n", values.constAt(x, y)))
                return false;
        if (!parser.writeLineF("\n"))
            return false;
    }

    return true;
}

// Export Data2D as cartesian data
bool Data2DExportFileFormat::exportCartesian(LineParser &parser, const Data2D &data)
{
    // Three-column format (x  y  value) in blocks of similar y value, separated by blank lines
    const Array2D<double> &values = data.constValues2D();
    const auto &xAxis = data.constXAxis();
    const auto &yAxis = data.constYAxis();
    for (int x = 0; x < values.nRows(); ++x)
    {
        for (int y = 0; y < values.nColumns(); ++y)
            if (!parser.writeLineF("{:15.9f} {:15.9f} {:15.9f}\n", xAxis.constAt(x), yAxis.constAt(y), values.constAt(x, y)))
                return false;
        if (!parser.writeLineF("\n"))
            return false;
    }

    return true;
}

// Export Data2D using current filename and format
bool Data2DExportFileFormat::exportData(const Data2D &data)
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
    if (data2DFormat() == Data2DExportFileFormat::BlockData2D)
        result = exportBlock(parser, data);
    // 	else if (data2DFormat() == Data2DExportFileFormat::CartesianData2D) result = exportCartesian(parser, data);
    else
    {
        Messenger::error("Unrecognised Data2D format.\nKnown formats are:\n");
        printAvailableFormats();
    }

    return result;
}

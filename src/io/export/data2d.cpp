// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

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
EnumOptions<Data2DExportFileFormat::Data2DExportFormat> Data2DExportFileFormat::data2DExportFormats()
{
    return EnumOptions<Data2DExportFileFormat::Data2DExportFormat>(
        "Data2DExportFileFormat", {{Data2DExportFileFormat::BlockData2D, "block", "Block Data"},
                                   {Data2DExportFileFormat::CartesianData2D, "cartesian", "Cartesian (x,y,value) Data"}});
}

// Return number of available formats
int Data2DExportFileFormat::nFormats() const { return Data2DExportFileFormat::nData2DExportFormats; }

// Return format keyword for supplied index
std::string Data2DExportFileFormat::formatKeyword(int id) const { return data2DExportFormats().keywordByIndex(id); }

// Return description string for supplied index
std::string Data2DExportFileFormat::formatDescription(int id) const { return data2DExportFormats().descriptionByIndex(id); }

// Return current format as CoordinateExportFormat
Data2DExportFileFormat::Data2DExportFormat Data2DExportFileFormat::data2DFormat() const
{
    return (Data2DExportFileFormat::Data2DExportFormat)format_;
}

/*
 * Export Functions
 */

// Export Data2D as simple block data
bool Data2DExportFileFormat::exportBlock(LineParser &parser, const std::vector<double> &xAxis, const std::vector<double> &yAxis,
                                         const Array2D<double> &values, OptionalReferenceWrapper<const Array2D<double>> errors)
{
    // Export header comment
    if (!parser.writeLineF("# {} blocks (nX) of {} points (nY).\n", xAxis.size(), yAxis.size()))
        return false;

    // Export datapoints, separating each block of a specific x value with a single blank line
    for (auto x = 0; x < values.nRows(); ++x)
    {
        for (auto y = 0; y < values.nColumns(); ++y)
            if (!parser.writeLineF("{:15.9f}\n", values[{x, y}]))
                return false;
        if (!parser.writeLineF("\n"))
            return false;
    }

    return true;
}

// Export Data2D as cartesian data
bool Data2DExportFileFormat::exportCartesian(LineParser &parser, const std::vector<double> &xAxis,
                                             const std::vector<double> &yAxis, const Array2D<double> &values,
                                             OptionalReferenceWrapper<const Array2D<double>> errors)
{
    // Three-column format (x  y value) in blocks of similar y value, separated by blank lines
    for (auto x = 0; x < values.nRows(); ++x)
    {
        for (auto y = 0; y < values.nColumns(); ++y)
            if (!parser.writeLineF("{:15.9f} {:15.9f} {:15.9f}\n", xAxis[x], yAxis[y], values[{x, y}]))
                return false;
        if (!parser.writeLineF("\n"))
            return false;
    }

    return true;
}

// Export Data2D using current filename and format
bool Data2DExportFileFormat::exportData(const Data2DBase &data)
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
        result = exportBlock(parser, data.xAxis(), data.yAxis(), data.values());
    else if (data2DFormat() == Data2DExportFileFormat::CartesianData2D)
        result = exportCartesian(parser, data.xAxis(), data.yAxis(), data.values());
    else
    {
        Messenger::error("Unrecognised Data2D format.\nKnown formats are:\n");
        printAvailableFormats();
    }

    return result;
}

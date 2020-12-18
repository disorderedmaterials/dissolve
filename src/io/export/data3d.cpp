// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#include "io/export/data3d.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "math/data3d.h"

Data3DExportFileFormat::Data3DExportFileFormat(std::string_view filename, Data3DExportFormat format)
    : FileAndFormat(filename, format)
{
}

/*
 * Format Access
 */

// Return enum options for Data3DExportFormat
EnumOptions<Data3DExportFileFormat::Data3DExportFormat> &Data3DExportFileFormat::data3DExportFormats()
{
    static EnumOptionsList Data3DExportFormats =
        EnumOptionsList() << EnumOption(Data3DExportFileFormat::BlockData3D, "block", "Block Data")
                          << EnumOption(Data3DExportFileFormat::CartesianData3D, "cartesian", "Cartesian (x,y,z,value) Data")
                          << EnumOption(Data3DExportFileFormat::PDensData3D, "pdens", "DLPutils PDens Data");

    static EnumOptions<Data3DExportFileFormat::Data3DExportFormat> options("Data3DExportFileFormat", Data3DExportFormats);

    return options;
}

// Return number of available formats
int Data3DExportFileFormat::nFormats() const { return Data3DExportFileFormat::nData3DExportFormats; }

// Return format keyword for supplied index
std::string_view Data3DExportFileFormat::formatKeyword(int id) const { return data3DExportFormats().keywordByIndex(id); }

// Return description string for supplied index
std::string_view Data3DExportFileFormat::formatDescription(int id) const
{
    return data3DExportFormats().descriptionByIndex(id);
}

// Return current format as CoordinateExportFormat
Data3DExportFileFormat::Data3DExportFormat Data3DExportFileFormat::data3DFormat() const
{
    return (Data3DExportFileFormat::Data3DExportFormat)format_;
}

/*
 * Export Functions
 */

// Export Data3D as simple block data
bool Data3DExportFileFormat::exportBlock(LineParser &parser, const Data3D &data)
{
    // Export header comment
    if (!parser.writeLineF("# {} blocks (nX*nY) of {} points (nZ).\n", data.xAxis().size() * data.yAxis().size(),
                           data.zAxis().size()))
        return false;

    // Export datapoints, separating each block of a specific x value with a single blank line
    const Array3D<double> &values = data.values3D();
    for (auto x = 0; x < values.nX(); ++x)
    {
        for (auto y = 0; y < values.nY(); ++y)
        {
            for (auto z = 0; z < values.nZ(); ++z)
            {
                if (!parser.writeLineF("{:15.9e}\n", values[{x, y, z}]))
                    return false;
            }
            if (!parser.writeLineF("\n"))
                return false;
        }
    }

    return true;
}

// Export Data3D as cartesian data
bool Data3DExportFileFormat::exportCartesian(LineParser &parser, const Data3D &data)
{
    // Four-column format (x  y  z  value) in blocks of similar x and y value, separated by blank lines
    const Array3D<double> &values = data.values3D();
    const auto &xAxis = data.xAxis();
    const auto &yAxis = data.yAxis();
    const auto &zAxis = data.zAxis();
    for (auto x = 0; x < values.nX(); ++x)
    {
        double xVal = xAxis[x];
        for (auto y = 0; y < values.nY(); ++y)
        {
            double yVal = yAxis[y];
            for (auto z = 0; z < values.nZ(); ++z)
            {
                if (!parser.writeLineF("{:15.9e} {:15.9e} {:15.9e} {:15.9e}\n", xVal, yVal, zAxis[z], values[{x, y, z}]))
                    return false;
            }
        }
        if (!parser.writeLineF("\n"))
            return false;
    }

    return true;
}

// Export Data3D as pdens data
bool Data3DExportFileFormat::exportPDens(LineParser &parser, const Data3D &data)
{
    // Line 1 (Integer Extents): nx, ny, nz, xmin, ymin, zmin, xmax, ymax, zmax
    const Array3D<double> &values = data.values3D();
    if (!parser.writeLineF("{:5d}{:5d}{:5d}{:5d}{:5d}{:5d}{:5d}{:5d}{:5d}\n", values.nX(), values.nY(), values.nZ(), 0, 0, 0,
                           values.nX(), values.nY(), values.nZ()))
        return false;

    // Line 2 (Axis Definitions) - assume orthogonal
    if (!parser.writeLineF("{:9.3e} {:9.3e} {:9.3e} {:9.3e} {:9.3e} {:9.3e} {:9.3e} {:9.3e} {:9.3e}\n",
                           data.xAxis()[1] - data.xAxis()[0], 0.0, 0.0, 0.0, data.yAxis()[1] - data.yAxis()[0], 0.0, 0.0, 0.0,
                           data.zAxis()[1] - data.zAxis()[0]))
        return false;

    // Line 3 (Origin)
    if (!parser.writeLineF("{:10.4f}{:10.4f}{:10.4f}\n", data.xAxis()[0], data.yAxis()[0], data.zAxis()[0]))
        return false;

    // Line 4 (Loop order)
    if (!parser.writeLineF("zyx\n"))
        return false;

    // Lines 5+ (Data)
    for (auto x = 0; x < values.nX(); ++x)
    {
        for (auto y = 0; y < values.nY(); ++y)
        {
            for (auto z = 0; z < values.nZ(); ++z)
            {
                if (!parser.writeLineF("{:15.9e}\n", values[{x, y, z}]))
                    return false;
            }
        }
    }

    return true;
}

// Export Data3D using current filename and format
bool Data3DExportFileFormat::exportData(const Data3D &data)
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
    if (data3DFormat() == Data3DExportFileFormat::BlockData3D)
        result = exportBlock(parser, data);
    else if (data3DFormat() == Data3DExportFileFormat::CartesianData3D)
        result = exportCartesian(parser, data);
    else if (data3DFormat() == Data3DExportFileFormat::PDensData3D)
        result = exportPDens(parser, data);
    else
    {
        Messenger::error("Unrecognised Data3D format.\nKnown formats are:\n");
        printAvailableFormats();
    }

    return result;
}

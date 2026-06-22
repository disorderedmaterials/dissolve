// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/exportDLPUtilsPDensData.h"
#include "base/lineParser.h"

ExportDLPUtilsPDensDataNode::ExportDLPUtilsPDensDataNode(Graph *parentGraph) : Node(parentGraph)
{
    // Inputs
    addInput("Data", "Data to export", data_);

    // Options
    addOption<std::string>("FilePath", "Destination file path", filePath_);
}

/*
 * Definition
 */

std::string_view ExportDLPUtilsPDensDataNode::type() const { return "ExportPDensData"; }

std::string_view ExportDLPUtilsPDensDataNode::summary() const { return "Export 3D data in DLPUtils PDens format"; }

/*
 * Processing
 */

// Perform processing
NodeConstants::ProcessResult ExportDLPUtilsPDensDataNode::process()
{
    return write(data_, filePath_) ? NodeConstants::ProcessResult::Success : NodeConstants::ProcessResult::Failed;
}

// Write data specified
bool ExportDLPUtilsPDensDataNode::write(const Data3D &data, const std::string &filePath)
{
    // Open the file
    LineParser parser;
    if (!parser.openOutput(filePath))
    {
        parser.closeFiles();
        return false;
    }

    const auto &xAxis = data.xAxis();
    const auto &yAxis = data.xAxis();
    const auto &zAxis = data.zAxis();
    const auto &values = data.values();

    // Line 1 (Integer Extents): nx, ny, nz, xmin, ymin, zmin, xmax, ymax, zmax
    if (!parser.writeLineF("{:5d}{:5d}{:5d}{:5d}{:5d}{:5d}{:5d}{:5d}{:5d}\n", values.nX(), values.nY(), values.nZ(), 0, 0, 0,
                           values.nX(), values.nY(), values.nZ()))
        return false;

    // Line 2 (Axis Definitions) - assume orthogonal
    if (!parser.writeLineF("{:9.3e} {:9.3e} {:9.3e} {:9.3e} {:9.3e} {:9.3e} {:9.3e} {:9.3e} {:9.3e}\n", xAxis[1] - xAxis[0],
                           0.0, 0.0, 0.0, yAxis[1] - yAxis[0], 0.0, 0.0, 0.0, zAxis[1] - zAxis[0]))
        return false;

    // Line 3 (Origin)
    if (!parser.writeLineF("{:10.4f}{:10.4f}{:10.4f}\n", xAxis[0], yAxis[0], zAxis[0]))
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
                if (!parser.writeLineF("{:15.9e}\n", values[std::tuple{x, y, z}]))
                    return false;
            }
        }
    }

    return true;
}
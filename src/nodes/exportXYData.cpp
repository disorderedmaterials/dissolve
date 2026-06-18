// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/exportXYData.h"
#include "base/lineParser.h"
#include "math/data1D.h"

ExportXYDataNode::ExportXYDataNode(Graph *parentGraph) : Node(parentGraph)
{
    // Inputs
    addInput("Data", "Imported data", data_);

    // Options
    addOption<std::string>("FilePath", "Destination file path", filePath_);
}

std::string_view ExportXYDataNode::type() const { return "ExportXYData"; }

std::string_view ExportXYDataNode::summary() const { return "Export XY data."; }

/*
 * Processing
 */

NodeConstants::ProcessResult ExportXYDataNode::process()
{
    return write(*data_, filePath_) ? NodeConstants::ProcessResult::Success : NodeConstants::ProcessResult::Failed;
}

// Write data specified
bool ExportXYDataNode::write(const Data1D &data, const std::string &filePath)
{
    // Open the file
    LineParser parser;
    if (!parser.openOutput(filePath))
    {
        parser.closeFiles();
        return false;
    }

    // Write data
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
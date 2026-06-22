// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/exportData.h"
#include "base/lineParser.h"

ExportDataNode::ExportDataNode(Graph *parentGraph) : Node(parentGraph)
{
    // Inputs
    addInput("Data", "Data to export", data_);

    // Options
    addOption("FilePath", "Destination file path", filePath_);
}

/*
 * Definition
 */

// Return type of the node
std::string_view ExportDataNode::type() const { return "ExportData"; }

// Return short summary of the node's purpose
std::string_view ExportDataNode::summary() const { return "Export data in a simple multi-column format"; }

/*
 * Processing
 */

// Perform processing
NodeConstants::ProcessResult ExportDataNode::process()
{
    return std::visit(
        [&](auto &&arg)
        {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::monostate>)
                return NodeConstants::ProcessResult::Failed;
            else if constexpr (std::is_same_v<T, Data1D>)
                return write(arg, filePath_) ? NodeConstants::ProcessResult::Success : NodeConstants::ProcessResult::Failed;
            else if constexpr (std::is_same_v<T, Data2D>)
                return write(arg, filePath_) ? NodeConstants::ProcessResult::Success : NodeConstants::ProcessResult::Failed;
            else if constexpr (std::is_same_v<T, Data3D>)
                return write(arg, filePath_) ? NodeConstants::ProcessResult::Success : NodeConstants::ProcessResult::Failed;
            else
                static_assert(false, "Visitor doesn't cater for all possible types.");

            return NodeConstants::ProcessResult::Success;
        },
        data_.data);
}

// Write data specified
bool ExportDataNode::write(const Data1D &data, const std::string &filePath)
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
bool ExportDataNode::write(const Data2D &data, const std::string &filePath)
{
    // Open the file
    LineParser parser;
    if (!parser.openOutput(filePath))
    {
        parser.closeFiles();
        return false;
    }

    // Three/four-column format (x y value [error]) in blocks of similar x value, separated by blank lines
    const auto &xAxis = data.xAxis();
    const auto &yAxis = data.xAxis();
    const auto &values = data.values();
    if (data.valuesHaveErrors())
    {
        const auto &errors = data.errors();
        for (auto x = 0; x < xAxis.size(); ++x)
        {
            for (auto y = 0; y < yAxis.size(); ++y)
                if (!parser.writeLineF("{:16.10f} {:16.10f} {:16.10f} {:16.10f}\n", xAxis[x], yAxis[y], values[{x, y}],
                                       errors[{x, y}]))
                    return false;
            if (!parser.writeLineF("\n"))
                return false;
        }
    }
    else
    {
        for (auto x = 0; x < xAxis.size(); ++x)
        {
            for (auto y = 0; y < yAxis.size(); ++y)
                if (!parser.writeLineF("{:16.10f} {:16.10f} {:16.10f}\n", xAxis[x], yAxis[y], values[{x, y}]))
                    return false;
            if (!parser.writeLineF("\n"))
                return false;
        }
    }

    return true;
}
bool ExportDataNode::write(const Data3D &data, const std::string &filePath)
{
    // Open the file
    LineParser parser;
    if (!parser.openOutput(filePath))
    {
        parser.closeFiles();
        return false;
    }

    // Three/four-Four/five-column format (x y z value [error]) in blocks of similar x and y value, separated by blank lines
    const auto &xAxis = data.xAxis();
    const auto &yAxis = data.xAxis();
    const auto &zAxis = data.zAxis();
    const auto &values = data.values();
    if (data.valuesHaveErrors())
    {
        const auto &errors = data.errors();
        for (auto x = 0; x < xAxis.size(); ++x)
        {
            for (auto y = 0; y < yAxis.size(); ++y)
            {
                for (auto z = 0; z < zAxis.size(); ++z)
                {
                    if (!parser.writeLineF("{:16.10f} {:16.10f} {:16.10f} {:16.10f} {:16.10f}\n", xAxis[x], yAxis[y], zAxis[z],
                                           values[{x, y, z}], errors[{x, y, z}]))
                        return false;
                    if (!parser.writeLineF("\n"))
                        return false;
                }
            }
        }
    }
    else
    {
        for (auto x = 0; x < xAxis.size(); ++x)
        {
            for (auto y = 0; y < yAxis.size(); ++y)
            {
                for (auto z = 0; z < zAxis.size(); ++z)
                {
                    if (!parser.writeLineF("{:16.10f} {:16.10f} {:16.10f} {:16.10f}\n", xAxis[x], yAxis[y], zAxis[z],
                                           values[{x, y, z}]))
                        return false;
                    if (!parser.writeLineF("\n"))
                        return false;
                }
            }
        }
    }

    return true;
}
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/exportBlockData.h"
#include "base/lineParser.h"

ExportBlockDataNode::ExportBlockDataNode(Graph *parentGraph) : Node(parentGraph)
{
    // Inputs
    addInput("Data", "Data to export", data_);

    // Options
    addOption("FilePath", "Destination file path", filePath_);
}

// Return type of the node
std::string_view ExportBlockDataNode::type() const { return "ExportBlockData"; }

// Return short summary of the node's purpose
std::string_view ExportBlockDataNode::summary() const { return "Export data values in block format"; }

/*
 * Processing
 */

NodeConstants::ProcessResult ExportBlockDataNode::process()
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

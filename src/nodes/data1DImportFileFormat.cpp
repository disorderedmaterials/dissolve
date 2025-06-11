// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "nodes/data1DImportFileFormat.h"

Data1DImportFileFormatNode::Data1DImportFileFormatNode(Graph *parentGraph) : Node(parentGraph)
{
    addOption<std::string>("Filename", "Filename", filename_);
    addOption<Data1DImportFileFormat::Data1DImportFormat>("Format", "Format", format_);
    addOption<std::optional<double>>("RemoveAverageFromX", "X value from which to calculate average Y and remove",
                                     removeAverageFromX_);
    addOption<std::optional<double>>("XMin", "Minimum x value to permit when reading data", xMin_);
    addOption<std::optional<double>>("XMax", "Maximum x value to permit when reading data", xMax_);
    addOption<int>("XColumn", "Column index to use for x values", xColumn_);
    addOption<int>("YColumn", "Column index to use for y values", yColumn_);
    addOption<int>("ErrorColumn", "Column index to use for error values", errorColumn_);
    addOption<int>("NPointsToRemove", "Remove a number of points from the start of the data", nPointsToRemove_);
    addOutput<Data1DImportFileFormat>("Data1DImportFileFormat", "Data1D import file format", *data1DImportFileFormat_);
}

std::string_view Data1DImportFileFormatNode::type() const { return "Data1DImportFileFormat"; }

std::string_view Data1DImportFileFormatNode::summary() const { return "Data1D import formats."; }

NodeConstants::ProcessResult Data1DImportFileFormatNode::process()
{
    data1DImportFileFormat_.emplace(filename_, format_, xColumn_, yColumn_, errorColumn_);
    return NodeConstants::ProcessResult::Success;
}

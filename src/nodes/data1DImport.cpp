// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "nodes/data1DImport.h"
#include "nodes/dissolve.h"

Data1DImportNode::Data1DImportNode(Graph *parentGraph) : Node(parentGraph)
{
    addOption<std::string>("Filepath", "File path", filePath_);
    addOption<Data1DImportFileFormat::Data1DImportFormat>("ImportFormat", "Format of imported data", format_);
    addOption<std::optional<Number>>("RemoveAverageFromX", "Remove average from X values", removeAverageFromX_);
    addOption<std::optional<Number>>("XMin", "Minimum X value", xMin_);
    addOption<std::optional<Number>>("XMax", "Maximum X value", xMax_);
    addOption<Number>("NPointsToRemove", "Number of points to remove", nPointsToRemove_);
    addOption<Number>("XColumn", "Column index of data X values", xColumn_);
    addOption<Number>("YColumn", "Column index of data Y values", yColumn_);
    addOption<Number>("ErrorColumn", "Column index of data error values", errorColumn_);
    addOutput<Data1D>("Data", "Imported data", data_);
}

std::string_view Data1DImportNode::type() const { return "Data1DImport"; }

std::string_view Data1DImportNode::summary() const { return "Import 1D data."; }

NodeConstants::ProcessResult Data1DImportNode::process()
{
    auto removeAverageFromX = removeAverageFromX_.value().asDouble(), xMin = xMin_.value().asDouble(),
         xMax = xMax_.value().asDouble();
    auto xColumn = xColumn_.asInteger(), yColumn = yColumn_.asInteger(), errorColumn = errorColumn_.asInteger(),
         nPointsToRemove = nPointsToRemove_.asInteger();
    Data1DImportFileFormat dataImport(filePath_, format_, removeAverageFromX, xMin, xMax, nPointsToRemove, xColumn, yColumn,
                                      errorColumn);

    if (dataImport.importData(data_))
        return NodeConstants::ProcessResult::Success;

    return NodeConstants::ProcessResult::Failed;
}

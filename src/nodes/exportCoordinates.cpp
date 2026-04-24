// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "io/export/coordinates.h"
#include "main/dissolve.h"
#include "nodes/constants.h"
#include "nodes/exportCoordinates.h"
#include <fstream>

ExportCoordinatesNode::ExportCoordinatesNode(Graph *parentGraph) : Node(parentGraph)
{
    // Inputs
    addInput<Configuration *>("Configuration", "Configuration from which Coordinates will be exported", configuration_);

    // Options
    addOption<std::string>("FilePath", "File path", filePath_);
    addOption<bool>("TagWithIteration", "Whether to tag (suffix) the filename with the current iteration index", tagWithIteration_);
    addOption<CoordinateExportFormat>("Format", "Format for coordinates", format_);
}

std::string_view ExportCoordinatesNode::type() const { return "ExportCoordinates"; }

std::string_view ExportCoordinatesNode::summary() const
{
    return "Export configuration coordinates from sequential frames of a coordinates.";
}

NodeConstants::ProcessResult ExportCoordinatesNode::process()
{

    auto path = filePath_;
    if (tagWithIteration_)
      path = std::format("{}.{}", path, dissolve().iteration());

    switch (format_)
    {
        case CoordinateExportFormat::XYZ:
          return exportXYZ(path);
        case CoordinateExportFormat::DLPOLY:
          return exportDLPoly(path);
        default:
          return NodeConstants::ProcessResult::Failed;
    }

    std::ofstream outfile(filePath_);
    std::ostream_iterator<char> out(outfile);

    std::format_to(out, "{}\n", configuration_->nAtoms());
    std::format_to(out, "{} @ {}\n", configuration_->name(), configuration_->version());

    outfile.close();

    return NodeConstants::ProcessResult::Success;
}

NodeConstants::ProcessResult ExportCoordinatesNode::exportXYZ(std::string path)
{
    std::ofstream outfile(filePath_);
    std::ostream_iterator<char> out(outfile);

    outfile.close();

    return NodeConstants::ProcessResult::Success;
}

NodeConstants::ProcessResult ExportCoordinatesNode::exportDLPoly(std::string path)
{
    std::ofstream outfile(filePath_);
    std::ostream_iterator<char> out(outfile);

    outfile.close();

    return NodeConstants::ProcessResult::Success;
}

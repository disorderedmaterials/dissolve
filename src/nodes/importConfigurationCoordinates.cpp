// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/importConfigurationCoordinates.h"

ImportConfigurationCoordinatesNode::ImportConfigurationCoordinatesNode(Graph *parentGraph) : Node(parentGraph)
{
    // Inputs
    addInput<Configuration *>("Configuration", "Configuration to which coordinates will be imported", configuration_);

    // Options
    addOption<std::string>("FilePath", "File path", filePath_);
    addOption<CoordinateImportFileFormat::CoordinateImportFormat>("FileFormat", "File format", format_);

    // Outputs
    addOutput<Configuration *>("Configuration", "Output configuration", configuration_);
}

std::string_view ImportConfigurationCoordinatesNode::type() const { return "ImportCoordinates"; }

std::string_view ImportConfigurationCoordinatesNode::summary() const { return "Import coordinates from a file."; }

NodeConstants::ProcessResult ImportConfigurationCoordinatesNode::process()
{
    CoordinateImportFileFormat fileSource(filePath_, format_);

    return fileSource.importData(configuration_) ? NodeConstants::ProcessResult::Success : NodeConstants::ProcessResult::Failed;
}

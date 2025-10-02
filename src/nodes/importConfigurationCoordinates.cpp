// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "nodes/importConfigurationCoordinates.h"
#include "nodes/dissolve.h"

ImportConfigurationCoordinatesNode::ImportConfigurationCoordinatesNode(Graph *parentGraph) : Node(parentGraph)
{
    addInput<Configuration *>("Configuration", "Configuration to which coordinates will be imported", configuration_);
    addOption<std::string>("FilePath", "File path", filePath_);
    addOption<CoordinateImportFileFormat::CoordinateImportFormat>("FileFormat", "File format", format_);
    addOutput<Configuration *>("Configuration", "Output configuration", configuration_);
}

std::string_view ImportConfigurationCoordinatesNode::type() const { return "ImportCoordinates"; }

std::string_view ImportConfigurationCoordinatesNode::summary() const { return "Import coordinates from a file."; }

NodeConstants::ProcessResult ImportConfigurationCoordinatesNode::process()
{
    CoordinateImportFileFormat fileSource(filePath_, format_);

    if (fileSource.importData(configuration_))
        return NodeConstants::ProcessResult::Success;

    return NodeConstants::ProcessResult::Unchanged;
}

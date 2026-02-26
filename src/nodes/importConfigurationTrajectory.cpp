// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/importConfigurationTrajectory.h"

ImportConfigurationTrajectoryNode::ImportConfigurationTrajectoryNode(Graph *parentGraph) : Node(parentGraph)
{
    // Inputs
    addInput<Configuration *>("Configuration", "Configuration to which Trajectory will be imported", configuration_);

    // Options
    addOption<std::string>("FilePath", "File path", filePath_);
    addOption<TrajectoryImportFileFormat::TrajectoryImportFormat>("FileFormat", "File format", format_);

    // Outputs
    addOutput<Configuration *>("Configuration", "Output configuration", configuration_);

    // Serialisable data
    // addSerialisable("filePosition", filePosition_);
}

std::string_view ImportConfigurationTrajectoryNode::type() const { return "ImportTrajectory"; }

std::string_view ImportConfigurationTrajectoryNode::summary() const
{
    return "Import configuration coordinates from sequential frames of a trajectory.";
}

NodeConstants::ProcessResult ImportConfigurationTrajectoryNode::process()
{
    TrajectoryImportFileFormat format(filePath_, format_);

    message("Import: Reading trajectory file frame from '{}' into Configuration '{}'...\n", format.filename(),
            configuration_->name());

    // Open the file
    LineParser parser;
    if ((!parser.openInput(format.filename())) || (!parser.isFileGoodForReading()))
    {
        error("Couldn't open trajectory file '{}'.\n", format.filename());
        return NodeConstants::ProcessResult::Failed;
    }

    // Seek to the next file position
    parser.seekg(filePosition_);

    // Read the frame
    std::optional<Matrix3> unitCell;
    if (!format.importData(parser, configuration_, unitCell))
    {
        error("Failed to read trajectory frame data.\n");
        return NodeConstants::ProcessResult::Failed;
    }

    configuration_->notifyAtomicPositionsChanged();

    // Store the new trajectory file position
    filePosition_ = parser.tellg();

    // Set the unit cell if one was read in
    if (unitCell)
        configuration_->createBox(unitCell.value());

    return NodeConstants::ProcessResult::Success;
}

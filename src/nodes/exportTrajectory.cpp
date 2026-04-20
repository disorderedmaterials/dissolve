// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/exportTrajectory.h"

ExportTrajectoryNode::ExportTrajectoryNode(Graph *parentGraph) : Node(parentGraph)
{
    // Inputs
    addInput<Configuration *>("Configuration", "Configuration from which Trajectory will be exported", configuration_);

    // Options
    addOption<std::string>("FilePath", "File path", filePath_);
    addOption<TrajectoryExportFileFormat::TrajectoryExportFormat>("FileFormat", "File format", format_);
    addOption<bool>("Extended", "Extended XYZ format", extended_);

    // Serialisable data
    // addSerialisable("filePosition", filePosition_);
    addOption<bool>("Extended", "Extended XYZ format", extended_);
}

std::string_view ExportTrajectoryNode::type() const { return "ExportTrajectory"; }

std::string_view ExportTrajectoryNode::summary() const
{
    return "Export configuration coordinates from sequential frames of a trajectory.";
}

NodeConstants::ProcessResult ExportTrajectoryNode::process()
{
    TrajectoryExportFileFormat format(filePath_, format_);
    return NodeConstants::ProcessResult::Failed;
}

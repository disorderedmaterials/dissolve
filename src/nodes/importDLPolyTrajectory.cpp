// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/importDLPolyTrajectory.h"
#include "nodes/importDLPolyStructure.h"

ImportDLPolyTrajectoryNode::ImportDLPolyTrajectoryNode(Graph *parentGraph) : Node(parentGraph)
{
    // Options
    addOption<std::string>("FilePath", "File path", filePath_);

    // Outputs
    addOutput<Structure>("Structure", "Imported structure", structure_);

    // Serialisable data
    // addSerialisable("filePosition", filePosition_);
}

std::string_view ImportDLPolyTrajectoryNode::type() const { return "ImportTrajectory"; }

std::string_view ImportDLPolyTrajectoryNode::summary() const
{
    return "Import configuration coordinates from sequential frames of a trajectory.";
}

NodeConstants::ProcessResult ImportDLPolyTrajectoryNode::process()
{
    message("Reading DL_POLY trajectory file frame from '{}'...\n", filePath_);

    // Open the file
    LineParser parser;
    if ((!parser.openInput(filePath_)) || (!parser.isFileGoodForReading()))
    {
        error("Couldn't open trajectory file '{}'.\n", filePath_);
        return NodeConstants::ProcessResult::Failed;
    }

    // Seek to the next file position
    parser.seekg(filePosition_);

    // Read the frame
    return ImportDLPolyStructureNode::read(parser, structure_);
}

// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/importXYZTrajectory.h"
#include "nodes/importXYZStructure.h"

ImportXYZTrajectoryNode::ImportXYZTrajectoryNode(Graph *parentGraph) : Node(parentGraph)
{
    volatile_ = true;

    // Options
    addOption("FilePath", "File path", filePath_);

    // Outputs
    addOutput("Structure", "Imported structure", structure_);

    // Serialisable data
    // addSerialisable("filePosition", filePosition_);
}

/*
 * Definition
 */

// Return type of the node
std::string_view ImportXYZTrajectoryNode::type() const { return "ImportXYZTrajectory"; }

// Return short summary of the node's purpose
std::string_view ImportXYZTrajectoryNode::summary() const
{
    return "Import structures from sequential frames of an XYZ trajectory";
}

/*
 * Processing
 */

// Perform processing
NodeConstants::ProcessResult ImportXYZTrajectoryNode::process()
{
    message("Reading XYZ trajectory file frame from '{}'...\n", filePath_);

    // Open the file
    LineParser parser;
    if ((!parser.openInput(filePath_)) || (!parser.isFileGoodForReading()))
    {
        error("Couldn't open trajectory file '{}'.\n", filePath_);
        return NodeConstants::ProcessResult::Failed;
    }

    // Seek to the next file position
    parser.seekg(filePosition_);

    structure_.clear();

    // Get the frame read result
    auto frameResult = ImportXYZStructureNode::read(parser, structure_);
    if (frameResult != NodeConstants::ProcessResult::Success)
        return frameResult;

    // Store the new trajectory file position
    filePosition_ = parser.tellg();

    return NodeConstants::ProcessResult::Success;
}

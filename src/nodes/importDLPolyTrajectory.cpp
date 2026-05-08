// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/importDLPolyTrajectory.h"
#include "nodes/importDLPolyStructure.h"

ImportDLPolyTrajectoryNode::ImportDLPolyTrajectoryNode(Graph *parentGraph) : Node(parentGraph)
{
    volatile_ = true;

    // Options
    addOption<std::string>("FilePath", "File path", filePath_);

    // Outputs
    addOutput<Structure>("Structure", "Imported structure", structure_);

    // Serialisable data
    // addSerialisable("filePosition", filePosition_);
}

std::string_view ImportDLPolyTrajectoryNode::type() const { return "ImportDLPolyTrajectory"; }

std::string_view ImportDLPolyTrajectoryNode::summary() const
{
    return "Import structures from sequential frames of a formatted DL_POLY trajectory.";
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

    // Read first line:  'timestep    <stepNo>  <nAtoms>  <keytrj>  <imcon>
    if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
        return NodeConstants::ProcessResult::Failed;

    // Get the frame read result
    auto frameResult = ImportDLPolyStructureNode::read(parser, parser.argi(3), parser.argi(4), parser.argi(2), structure_);
    if (frameResult != NodeConstants::ProcessResult::Success)
        return frameResult;

    // Store the new trajectory file position
    filePosition_ = parser.tellg();

    return NodeConstants::ProcessResult::Success;
}

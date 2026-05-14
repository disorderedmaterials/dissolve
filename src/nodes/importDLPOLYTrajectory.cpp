// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/importDLPOLYTrajectory.h"
#include "nodes/importDLPOLYStructure.h"

ImportDLPOLYTrajectoryNode::ImportDLPOLYTrajectoryNode(Graph *parentGraph) : Node(parentGraph)
{
    volatile_(true)

    // Options
    addOption<std::string>("FilePath", "File path", filePath_);

    // Outputs
    addOutput<Structure>("Structure", "Imported structure", structure_);

    // Serialisable data
    // addSerialisable("filePosition", filePosition_);
}

std::string_view ImportDLPOLYTrajectoryNode::type() const { return "ImportDLPOLYTrajectory"; }

std::string_view ImportDLPOLYTrajectoryNode::summary() const
{
    return "Import structures from sequential frames of a formatted DL_POLY trajectory.";
}

NodeConstants::ProcessResult ImportDLPOLYTrajectoryNode::process()
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

    auto keytrj = parser.argi(3);
    auto imcon = parser.argi(4);
    auto nAtoms = parser.hasArg(2) ? parser.argi(2) : 0;
    message(" --> Expecting coordinates for {} atoms (DLPOLY keytrj={}, imcon={}).\n", nAtoms, keytrj, imcon);

    // Get the frame read result
    auto frameResult = ImportDLPOLYStructureNode::read(parser, parser.argi(3), parser.argi(4), parser.argi(2), structure_);
    if (frameResult != NodeConstants::ProcessResult::Success)
        return frameResult;

    // Store the new trajectory file position
    filePosition_ = parser.tellg();

    return NodeConstants::ProcessResult::Success;
}

// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/importDLPOLYTrajectory.h"
#include "base/applicative.h"
#include "base/parserLibrary.h"
#include "nodes/importDLPOLYStructure.h"

ImportDLPOLYTrajectoryNode::ImportDLPOLYTrajectoryNode(Graph *parentGraph) : Node(parentGraph)
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
std::string_view ImportDLPOLYTrajectoryNode::type() const { return "ImportDLPOLYTrajectory"; }

// Return short summary of the node's purpose
std::string_view ImportDLPOLYTrajectoryNode::summary() const
{
    return "Import structures from sequential frames of a formatted DL_POLY trajectory";
}

/*
 * Processing
 */

// Perform processing
NodeConstants::ProcessResult ImportDLPOLYTrajectoryNode::process()
{
    using namespace Parsers;
    message("Reading DL_POLY trajectory file frame from '{}'...\n", filePath_);

    std::ifstream infile{filePath_};
    if (!infile)
    {
        error("Couldn't open trajectory file '{}'.\n", filePath_);
        return NodeConstants::ProcessResult::Failed;
    }

    // Seek to the next file position
    infile.seekg(filePosition_);

    // Read first line:  'timestep    <stepNo>  <nAtoms>  <keytrj>  <imcon>
    auto head = header().parse(infile);
    if (!head)
        return NodeConstants::ProcessResult::Failed;
    auto &[stepno, natoms, keytrj, imcon, _] = std::get<0>(*head);

    message(" --> Expecting coordinates for {} atoms (DLPOLY keytrj={}, imcon={}).\n", natoms, keytrj, imcon);

    // Get the frame read result
    auto frameResult = ImportDLPOLYStructureNode::read(infile, keytrj, imcon, natoms, structure_);
    if (frameResult != NodeConstants::ProcessResult::Success)
        return frameResult;

    // Store the new trajectory file position
    filePosition_ = infile.tellg();

    return NodeConstants::ProcessResult::Success;
}

Parsers::Parser<std::tuple<int, int, int, int, double>> ImportDLPOLYTrajectoryNode::header()
{
    using namespace Parsers;
    return literal("timestep") >> inlineSpaces() >> natural() << inlineSpaces() & natural() << inlineSpaces() &
           natural() << inlineSpaces() & natural() << inlineSpaces() & real() << spaces();
}

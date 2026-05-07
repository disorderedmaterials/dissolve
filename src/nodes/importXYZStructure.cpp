// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/importXYZStructure.h"

ImportXYZStructureNode::ImportXYZStructureNode(Graph *parentGraph) : Node(parentGraph)
{
    // Options
    addOption<std::string>("FilePath", "File path", filePath_);

    // Outputs
    addOutput<Structure>("Structure", "Imported structure", structure_);
}

std::string_view ImportXYZStructureNode::type() const { return "ImportXYZStructure"; }

std::string_view ImportXYZStructureNode::summary() const { return "Import an XYZ file."; }

NodeConstants::ProcessResult ImportXYZStructureNode::process()
{
    structure_.clear();

    // Open file and check that we're OK to proceed importing from it
    LineParser parser;
    if ((!parser.openInput(filePath_)) || (!parser.isFileGoodForReading()))
        return error("Couldn't open file '{}' for loading XYZ data.\n", filePath_);

    return read(parser, structure_);
}

// Read structure from the specified file parser
NodeConstants::ProcessResult ImportXYZStructureNode::read(LineParser &parser, Structure &structure)
{
    // Read natoms
    if (parser.getArgsDelim() != LineParser::Success)
        return NodeConstants::ProcessResult::Failed;
    auto nAtoms = parser.argi(0);

    // Skip title
    if (parser.skipLines(1) != LineParser::Success)
        return NodeConstants::ProcessResult::Failed;

    printf(" NAtoms = %d\n", nAtoms);
    for (auto n = 0; n < nAtoms; ++n)
    {
        if (parser.getArgsDelim() != LineParser::Success)
            return NodeConstants::ProcessResult::Failed;
        structure.addAtom(Elements::element(parser.argsv(0)), parser.arg3d(1), parser.hasArg(4) ? parser.argd(4) : 0.0);
    }
    printf("SUCCESS\n");
    return NodeConstants::ProcessResult::Success;
}
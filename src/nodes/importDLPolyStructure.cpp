// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/importDLPolyStructure.h"

ImportDLPolyStructureNode::ImportDLPolyStructureNode(Graph *parentGraph) : Node(parentGraph)
{
    // Options
    addOption<std::string>("FilePath", "File path", filePath_);

    // Outputs
    addOutput<Structure>("Structure", "Imported structure", structure_);
}

std::string_view ImportDLPolyStructureNode::type() const { return "ImportDLPolyStructure"; }

std::string_view ImportDLPolyStructureNode::summary() const { return "Import a DL_POLY CONFIG or REVCON file."; }

NodeConstants::ProcessResult ImportDLPolyStructureNode::process()
{
    /*
     * Import DL_POLY coordinates information through the specified line parser.
     * We assume CONFIG or REVCON format:
     *
     * Line 1:    Title
     * Line 2:    keytrj   imcon    natoms    []
     * Line 3-5:  cell matrix (if imcon > 0)
     * Line 6:    atomtype        id
     * Line 7:    rx   ry   rz
     * Line 8:    vx   vy   vz      if (keytrj > 0)
     * Line 9:    fx   fy   fz	if (keytrj > 1)
     *   ...
     */

    structure_.clear();

    // Open file and check that we're OK to proceed importing from it
    LineParser parser;
    if ((!parser.openInput(filePath_)) || (!parser.isFileGoodForReading()))
        return error("Couldn't open file '{}' for loading coordinates data.\n", filePath_);

    // Skip title
    if (parser.skipLines(1) != LineParser::Success)
        return NodeConstants::ProcessResult::Failed;

    // Import in keytrj, imcon, and number of atoms, and initialise arrays
    if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
        return NodeConstants::ProcessResult::Failed;

    auto keytrj = parser.argi(0);
    auto imcon = parser.argi(1);
    auto nAtoms = parser.hasArg(2) ? parser.argi(2) : 0;
    if (nAtoms == 0)
        Messenger::print(" --> Expecting coordinates for an unknown number of atoms (DLPOLY keytrj={}, imcon={}) - will read "
                         "until end of file.\n",
                         nAtoms, keytrj, imcon);
    else
        Messenger::print(" --> Expecting coordinates for {} atoms (DLPOLY keytrj={}, imcon={}).\n", nAtoms, keytrj, imcon);

    // Read cell information if given
    if (imcon > 0)
    {
        if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
            return NodeConstants::ProcessResult::Failed;
        auto m1 = parser.arg3d(0);
        if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
            return NodeConstants::ProcessResult::Failed;
        auto m2 = parser.arg3d(0);
        if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
            return NodeConstants::ProcessResult::Failed;
        auto m3 = parser.arg3d(0);
        structure_.createBox(Matrix3(m1, m2, m3));
    }

    // Loop over atoms (either a specified number, or until we reach the end of the file
    while (!parser.eofOrBlank())
    {
        // Skip atomname line, get the positions, then skip velocity and force lines if necessary
        if (parser.skipLines(1) != LineParser::Success)
            return NodeConstants::ProcessResult::Failed;
        if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
            return NodeConstants::ProcessResult::Failed;
        structure_.addAtom(Elements::Unknown, parser.arg3d(0));
        if (parser.skipLines(keytrj) != LineParser::Success)
            return NodeConstants::ProcessResult::Failed;
        if ((nAtoms > 0) && (structure_.nAtoms() == nAtoms))
            break;
    }

    return NodeConstants::ProcessResult::Success;
}

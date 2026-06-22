// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/importDLPOLYStructure.h"

ImportDLPOLYStructureNode::ImportDLPOLYStructureNode(Graph *parentGraph) : Node(parentGraph)
{
    // Options
    addOption<std::string>("FilePath", "File path", filePath_);

    // Outputs
    addOutput<Structure>("Structure", "Imported structure", structure_);
    addOutput("Forces", "Atomic forces (if present)", forces_);
}

/*
 * Definition
 */

std::string_view ImportDLPOLYStructureNode::type() const { return "ImportDLPOLYStructure"; }

std::string_view ImportDLPOLYStructureNode::summary() const { return "Import a DL_POLY CONFIG or REVCON file"; }

/*
 * Processing
 */

// Perform processing
NodeConstants::ProcessResult ImportDLPOLYStructureNode::process()
{
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
        message(" --> Expecting coordinates for an unknown number of atoms (DLPOLY keytrj={}, imcon={}) - will read "
                "until end of file.\n",
                nAtoms, keytrj, imcon);
    else
        message(" --> Expecting coordinates for {} atoms (DLPOLY keytrj={}, imcon={}).\n", nAtoms, keytrj, imcon);

    return read(parser, keytrj, imcon, nAtoms, structure_, forces_);
}

// Read structure from the specified file parser
NodeConstants::ProcessResult ImportDLPOLYStructureNode::read(LineParser &parser, int keytrj, int imcon, int nAtoms,
                                                             Structure &structure,
                                                             OptionalReferenceWrapper<std::vector<Vector3>> optForces)
{
    /*
     * Import DL_POLY coordinates information through the specified line parser.
     * We assume HISf, CONFIG or REVCON format (only the first two lines differ)
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

    // Clear storage objects
    structure.clear();
    std::vector<Vector3> forces;

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
        structure.createBox(Matrix3(m1, m2, m3));
    }

    // Loop over atoms (either a specified number, or until we reach the end of the file
    while (!parser.eofOrBlank())
    {
        // Skip atomname line
        if (parser.skipLines(1) != LineParser::Success)
            return NodeConstants::ProcessResult::Failed;

        // Read position
        if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
            return NodeConstants::ProcessResult::Failed;
        structure.addAtom(Elements::Unknown, parser.arg3d(0));

        // Read velocity if present
        if (keytrj > 0)
        {
            if (parser.skipLines(1) != LineParser::Success)
                return NodeConstants::ProcessResult::Failed;
        }

        // Read forces if present
        if (keytrj > 1)
        {
            if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
                return NodeConstants::ProcessResult::Failed;
            forces.push_back(parser.arg3d(0));
        }

        if ((nAtoms > 0) && (structure.nAtoms() == nAtoms))
            break;
    }

    // Copy forces out?
    if (optForces)
        optForces.value().get() = forces;

    return NodeConstants::ProcessResult::Success;
}

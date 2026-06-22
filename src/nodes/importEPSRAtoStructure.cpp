// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/importEPSRAtoStructure.h"

ImportEPSRAtoStructureNode::ImportEPSRAtoStructureNode(Graph *parentGraph) : Node(parentGraph)
{
    // Options
    addOption<std::string>("FilePath", "File path", filePath_);

    // Outputs
    addOutput<Structure>("Structure", "Imported structure", structure_);
}

/*
 * Definition
 */

// Return type of the node
std::string_view ImportEPSRAtoStructureNode::type() const { return "ImportEPSRAtoStructure"; }

// Return short summary of the node's purpose
std::string_view ImportEPSRAtoStructureNode::summary() const { return "Import an EPSR ato file"; }

/*
 * Processing
 */

// Perform processing
NodeConstants::ProcessResult ImportEPSRAtoStructureNode::process()
{
    structure_.clear();

    // Open file and check that we're OK to proceed importing from it
    LineParser parser;
    if ((!parser.openInput(filePath_)) || (!parser.isFileGoodForReading()))
        return error("Couldn't open file '{}' for loading EPSR ato data.\n", filePath_);

    // File header:
    // Either  1   : nmols, box length, temperature   (for cubic systems)
    //    or   2   : nmols,   temperature             (for non-cubic systems)
    // followed by : A, B, C
    //             : phib, thetac, phic
    if (parser.getArgsDelim() != LineParser::Success)
        return NodeConstants::ProcessResult::Failed;
    auto nMols = parser.argi(0);
    if (parser.nArgs() == 3)
    {
        double boxSize = parser.argd(1);
        message("File has a cubic cell (side length {} Angstroms)", boxSize);
        structure_.createBox({boxSize, boxSize, boxSize}, {90.0, 90.0, 90.0});
    }
    else
    {
        message("File has a full cell specification");
        Vector3 lengths, angles;
        if (parser.getArgsDelim() != LineParser::Success)
            return NodeConstants::ProcessResult::Failed;
        lengths = parser.arg3d(0);
        if (parser.getArgsDelim() != LineParser::Success)
            return NodeConstants::ProcessResult::Failed;
        angles = parser.arg3d(0);
        structure_.createBox(lengths, angles);
    }

    // 2 : step sizes etc. **IGNORED**
    if (parser.getArgsDelim() != LineParser::Success)
        return NodeConstants::ProcessResult::Failed;

    // Molecule/atom specifications are in the form:
    // n  : natoms, comx, comy, comz, phix, phiy, phiz
    // n+1: atom name 1
    // n+2: x,y,z (offsets from com)
    // n+3: nrestraints, res1, res2... (number of distance restraints, 5 per line)
    // n+4: ...resN-1, resN
    // n+5: nrot (number of defined molecular rotations)
    // n+6: atom1, atom2 (bonds of rotation 'axis')
    // n+7: list of headgroup atoms that are rotated
    auto atomOffset = 0;
    for (auto m = 0; m < nMols; m++)
    {
        Messenger::printVerbose("Importing molecule {} from EPSR ato file...\n", m + 1);

        if (parser.getArgsDelim() != LineParser::Success)
            return NodeConstants::ProcessResult::Failed;
        auto nAtoms = parser.argi(0);
        auto com = parser.arg3d(1);

        for (auto n = 0; n < nAtoms; n++)
        {
            // Atom name
            if (parser.getArgsDelim() != LineParser::Success)
                return NodeConstants::ProcessResult::Failed;
            auto name = parser.args(0);

            // Atom coordinates (specified as offset from com)
            if (parser.getArgsDelim() != LineParser::Success)
                return NodeConstants::ProcessResult::Failed;
            auto delta = parser.arg3d(0);

            // Add a new atom
            structure_.addAtom(name, com + delta);

            // Import in number of restraints line
            if (parser.getArgsDelim() != LineParser::Success)
                return NodeConstants::ProcessResult::Failed;
            auto nRestraints = parser.argi(0);
            auto currentArg = 1;
            while (nRestraints > 0)
            {
                // Look at next available argument - if none, import another line in
                if (currentArg >= parser.nArgs())
                {
                    if (parser.getArgsDelim() != LineParser::Success)
                        return NodeConstants::ProcessResult::Failed;
                    currentArg = 0;
                }
                currentArg += 2;
                --nRestraints;
            }
        }

        // Discard molecular rotations and dihedrals
        // There are 14 atoms per line - first line contains number of atoms followed by (up to) 13 indices
        if (parser.getArgsDelim() != LineParser::Success)
            return NodeConstants::ProcessResult::Failed;
        auto nRotations = parser.argi(0);
        while (nRotations > 0)
        {
            // Import line to find out which type of definition this is...
            if (parser.getArgsDelim() != LineParser::Success)
                return NodeConstants::ProcessResult::Failed;

            // Skip axis line
            if (parser.skipLines(1) != LineParser::Success)
                return NodeConstants::ProcessResult::Failed;

            // If a DIHedral, we expect an integer which defines the number of constraints, and thus the number of
            // lines to skip before the main
            if (DissolveSys::sameString(parser.argsv(0), "DIH"))
            {
                if (parser.getArgsDelim() != LineParser::Success)
                    return NodeConstants::ProcessResult::Failed;
                if (parser.skipLines(parser.argi(0)) != LineParser::Success)
                    return NodeConstants::ProcessResult::Failed;
            }

            // Finally, import in number of atoms affected by rotation and calculate next number of lines to discard
            if (parser.getArgsDelim() != LineParser::Success)
                return NodeConstants::ProcessResult::Failed;
            if (parser.skipLines(parser.argi(0) / 14) != LineParser::Success)
                return NodeConstants::ProcessResult::Failed;

            --nRotations;
        }

        atomOffset += nAtoms;
    }

    // Atomtype specifications follow, but we have the coordinates and so are done.
    // TODO We could parse the atom type to element mappings and back-convert our elements

    return NodeConstants::ProcessResult::Success;
}

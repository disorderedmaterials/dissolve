// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/importMoscitoStructure.h"

ImportMoscitoStructureNode::ImportMoscitoStructureNode(Graph *parentGraph) : Node(parentGraph)
{
    // Options
    addOption<std::string>("FilePath", "File path", filePath_);

    // Outputs
    addOutput<Structure>("Structure", "Imported structure", structure_);
    addOutput("Forces", "Atomic forces (if present)", forces_);
}

std::string_view ImportMoscitoStructureNode::type() const { return "ImportMoscitoStructure"; }

std::string_view ImportMoscitoStructureNode::summary() const { return "Import a Moscito file."; }

NodeConstants::ProcessResult ImportMoscitoStructureNode::process()
{
    /*
     * Import Moscito coordinate information through the specified line parser.
     * Structure file format is as follows (see http://139.30.122.11/MOSCITO/manual4.pdf):
     *
     * Line 1:    cx    cy    cz
     * Line 2:    nmolecules
     * Line 3:    empty / remark
     * Line 4:    molecule label
     * Line 5:    moltype  natoms  molindex
     * Line 6:    atomlabel   typeindex
     * Line 7:    rx   ry   rz
     * Line 8:    vx   vy   vz
     * Line 9:    fx   fy   fz
     *   ...
     *
     * Units are:  distance = nm, velocities = nm ps-1, forces = kJ mol-1 nm-1
     */

    // Clear storage objects
    structure_.clear();
    forces_.clear();

    // Open file and check that we're OK to proceed importing from it
    LineParser parser;
    if ((!parser.openInput(filePath_)) || (!parser.isFileGoodForReading()))
        return error("Couldn't open file '{}' for loading Moscito data.\n", filePath_);

    message(" --> Importing coordinates in Moscito (str) format...\n");
    // Read cell lengths
    if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
        return NodeConstants::ProcessResult::Failed;
    structure_.createBox(parser.arg3d(0), {90.0, 90.0, 90.0});

    // Read nmolecules
    if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
        return NodeConstants::ProcessResult::Failed;
    auto nMolecules = parser.argi(0);
    message(" --> Structure file contains {} molecules.\n", nMolecules);

    for (auto n = 0; n < nMolecules; ++n)
    {
        // Read and discard remark and molecule label lines
        if (parser.skipLines(2) != LineParser::Success)
            return NodeConstants::ProcessResult::Failed;

        // Get number of atoms in this molecule (second integer)
        if (parser.getArgsDelim(LineParser::KeepBlanks) != LineParser::Success)
            return NodeConstants::ProcessResult::Failed;
        auto nAtoms = parser.argi(1);

        // Read in atom coordinates
        for (auto i = 0; i < nAtoms; ++i)
        {
            // Read atom label / index line
            if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
                return NodeConstants::ProcessResult::Failed;
            auto name = parser.args(0);

            // Read coordinates (in nm)
            // Coordinates are in fixed format (15.8e) with *no spacing between values*
            if (parser.readNextLine(LineParser::Defaults) != LineParser::Success)
                return NodeConstants::ProcessResult::Failed;
            std::string line{parser.line()};
            structure_.addAtom(name, {std::stof(line.substr(0, 15)) * 10.0, std::stof(line.substr(15, 15)) * 10.0,
                                      std::stof(line.substr(30)) * 10.0});

            // Skip velocity line
            if (parser.skipLines(1) != LineParser::Success)
                return NodeConstants::ProcessResult::Failed;

            /*
             * Read forces (in kJ mol-1 nm-1)
             * Our internal units are 10 J mol-1 Angstrom-1:
             *
             *          kJ mol-1  =>  10 J mol-1   =   F * 100.0
             *              nm-1  =>  Angstroms-1  =   F / 10.0
             *
             * Final conversion factor =  * 10.0
             *
             * Note: Forces are in fixed format (15.8e) with *no spacing between values*
             */
            if (parser.readNextLine(LineParser::Defaults) != LineParser::Success)
                return NodeConstants::ProcessResult::Failed;
            line = parser.line();
            forces_.emplace_back(std::stof(line.substr(0, 15)) * 10.0, std::stof(line.substr(15, 15)) * 10.0,
                                 std::stof(line.substr(30)) * 10.0);
        }
    }

    return NodeConstants::ProcessResult::Success;
}

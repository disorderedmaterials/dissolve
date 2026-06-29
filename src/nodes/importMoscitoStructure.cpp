// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/importMoscitoStructure.h"
#include "base/parserLibrary.h"
#include <fstream>

ImportMoscitoStructureNode::ImportMoscitoStructureNode(Graph *parentGraph) : Node(parentGraph)
{
    // Options
    addOption("FilePath", "File path", filePath_);

    // Outputs
    addOutput("Structure", "Imported structure", structure_);
    addOutput("Forces", "Atomic forces (if present)", forces_);
}

/*
 * Definition
 */

// Return type of the node
std::string_view ImportMoscitoStructureNode::type() const { return "ImportMoscitoStructure"; }

// Return short summary of the node's purpose
std::string_view ImportMoscitoStructureNode::summary() const { return "Import a Moscito file"; }

/*
 * Processing
 */

// Perform processing
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
    using namespace Parsers;
    auto firstLine = inlineSpaces() >> vector3() << newlines();
    auto secondLine = inlineSpaces() >> natural() << newlines();
    auto thirdLine = maybe(inlines()) >> newlines();
    auto atom = alphanums() & inlineSpaces() >> natural() << spaces() & vector3() << spaces() & vector3() << spaces() &
                vector3() << spaces();
    auto molecule = alphanums() << spaces() & natural() & inlineSpaces() >> natural() &
                    inlineSpaces() >> natural() << newlines() & some(atom);
    auto fileStructure = firstLine & secondLine << thirdLine & some(molecule);

    std::ifstream infile(filePath_);
    if (!infile)
        return error("Couldn't open file '{}' for loading Moscito data.\n", filePath_);

    auto parsed = fileStructure.exact(infile);
    if (!parsed)
        return error("Couldn't parse file '{}' for loading Moscito data.\n", filePath_);

    auto &[box, nmolecules, molecules] = *parsed;

    // Clear storage objects
    structure_.clear();
    forces_.clear();

    structure_.createBox(box, {90.0, 90.0, 90.0});
    assert(nmolecules == molecules.size());
    for (auto molecule : molecules)
    {
        auto &[molname, moltype, natoms, molindex, atoms] = molecule;
        assert(natoms == atoms.size());

        // Read in atom coordinates
        for (auto atom : atoms)
        {
            auto &[name, index, pos, vec, force] = atom;
            structure_.addAtom(name, pos * 10.0);

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
            forces_.emplace_back(force * 10.0);
        }
    }

    return NodeConstants::ProcessResult::Success;
}

// parse the header of a moscito file
Parsers::Parser<std::tuple<Vector3, int>> ImportMoscitoStructureNode::header()
{
    using namespace Parsers;
    auto firstLine = inlineSpaces() >> vector3() << newlines();
    auto secondLine = inlineSpaces() >> natural() << newlines();
    auto thirdLine = maybe(inlines()) >> newlines();
    return firstLine & secondLine << thirdLine;
}

// parse an atom from a moscito file
Parsers::Parser<std::tuple<std::string, int, Vector3, Vector3, Vector3>> ImportMoscitoStructureNode::atom()
{
    using namespace Parsers;
    return alphanums() & inlineSpaces() >> natural() << spaces() & vector3() << spaces() & vector3() << spaces() &
           vector3() << spaces();
}
// parse an molecule from a moscito file
Parsers::Parser<std::tuple<std::string, int, int, int, std::vector<std::tuple<std::string, int, Vector3, Vector3, Vector3>>>>
ImportMoscitoStructureNode::molecule()
{
    using namespace Parsers;
    return alphanums() << spaces() & natural() & inlineSpaces() >> natural() & inlineSpaces() >> natural() << newlines() &
           some(ImportMoscitoStructureNode::atom());
}

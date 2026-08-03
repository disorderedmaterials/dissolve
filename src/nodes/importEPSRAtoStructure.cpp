// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/importEPSRAtoStructure.h"
#include "base/applicative.h"
#include "base/parserLibrary.h"

ImportEPSRAtoStructureNode::ImportEPSRAtoStructureNode(Graph *parentGraph) : Node(parentGraph)
{
    // Options
    addOption("FilePath", "File path", filePath_);

    // Outputs
    addOutput("Structure", "Imported structure", structure_);
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
    std::ifstream infile(filePath_);
    if (!infile)
        return error("Couldn't open file '{}' for loading EPSR ato data.\n", filePath_);

    // File header:
    // Either  1   : nmols, box length, temperature   (for cubic systems)
    //    or   2   : nmols,   temperature             (for non-cubic systems)
    // followed by : A, B, C
    //             : phib, thetac, phic
    using namespace Parsers;
    auto cubic = inlineSpaces() >> natural() & inlineSpaces() >> real() & inlineSpaces() >> real() << toEndOfLine();
    auto noncubic = inlineSpaces() >> natural() & pure(-1.0) & inlineSpaces() >> natural() << toEndOfLine();
    auto header = cubic | noncubic;
    auto head = header.parse(infile);
    if (!head)
        return NodeConstants::ProcessResult::Failed;
    auto &[nMols, boxSize, temperature] = std::get<0>(*head);
    if (boxSize != -1.0)
    {
        structure_.box() = Box({boxSize, boxSize, boxSize}, {90.0, 90.0, 90.0});
    }
    else
    {
        auto tempVect = inlineSpaces() >> vector3() << toEndOfLine();
        auto l = tempVect.parse(infile);
        if (!l)
            return NodeConstants::ProcessResult::Failed;
        auto lengths = std::get<0>(*l);
        auto a = tempVect.parse(infile);
        if (!a)
            return NodeConstants::ProcessResult::Failed;
        auto angles = std::get<0>(*a);
        structure_.box() = Box(lengths, angles);
    }

    // 2 : step sizes etc. **IGNORED**
    if (!toEndOfLine().parse(infile))
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
    auto m = 0;
    for (; m < nMols; m++)
    {
        Messenger::printVerbose("Importing molecule {} from EPSR ato file...\n", m + 1);
        auto molHeader =
            inlineSpaces() >> natural() & inlineSpaces() >> vector3() & inlineSpaces() >> vector3() << toEndOfLine();
        auto molHead = molHeader.parse(infile);

        if (!molHead)
            return NodeConstants::ProcessResult::Failed;
        auto &[nAtoms, com, phi] = std::get<0>(*molHead);

        auto n = 0;
        for (; n < nAtoms; n++)
        {
            auto atomHead = maybe(spaces()) >> alphas() << toEndOfLine();
            auto ah = atomHead.parse(infile);
            // Atom name
            if (!ah)
                return NodeConstants::ProcessResult::Failed;
            auto name = std::get<0>(*ah);

            auto del = (inlineSpaces() >> vector3() << toEndOfLine()).parse(infile);
            // Atom coordinates (specified as offset from com)
            if (!del)
                return NodeConstants::ProcessResult::Failed;
            auto delta = std::get<0>(*del);

            // Add a new atom
            structure_.addAtom(name, com + delta);

            // Import in number of restraints line
            auto restCount = (inlineSpaces() >> natural()).parse(infile);
            if (!restCount)
                return NodeConstants::ProcessResult::Failed;
            auto nRestraints = std::get<0>(*restCount);
            while (nRestraints > 0)
            {
                // Look at next available argument - if none, import another line in
                auto rest = (spaces() >> natural() & spaces() >> real());
                auto r = rest.parse(infile);
                if (!r)
                    return NodeConstants::ProcessResult::Failed;
                auto &[a, b] = std::get<0>(*r);
                --nRestraints;
            }
        }

        // Discard molecular rotations and dihedrals
        // There are 14 atoms per line - first line contains number of atoms followed by (up to) 13 indices
        auto rotations = (maybe(spaces()) >> natural() << toEndOfLine()).parse(infile);
        if (!rotations)
            return NodeConstants::ProcessResult::Failed;
        auto nRotations = std::get<0>(*rotations);
        while (nRotations > 0)
        {
            // If a DIHedral, we expect an integer which defines the number of constraints, and thus the number of
            // lines to skip before the main
            auto dih = maybe(inlineSpaces() >> "DIH"_p << toEndOfLine());
            auto d = dih.parse(infile);
            if (d && std::get<0>(*d))
            {
                toEndOfLine().parse(infile);
                auto constraints = (inlineSpaces() >> natural() << toEndOfLine()).parse(infile);
                if (!constraints)
                    return NodeConstants::ProcessResult::Failed;
                for (auto i = 0; i < std::get<0>(*constraints); ++i)
                    toEndOfLine().parse(infile);
            }

            // Finally, import in number of atoms affected by rotation and calculate next number of lines to discard
            auto atoms = (inlineSpaces() >> natural() << toEndOfLine()).parse(infile);
            if (!atoms)
                return NodeConstants::ProcessResult::Failed;
            auto ats = std::get<0>(*atoms);
            for (auto i = 0; i < ats / 14; ++i)
                toEndOfLine().parse(infile);

            --nRotations;
        }

        atomOffset += nAtoms;
    }

    // Atomtype specifications follow, but we have the coordinates and so are done.
    // TODO We could parse the atom type to element mappings and back-convert our elements

    return NodeConstants::ProcessResult::Success;
}

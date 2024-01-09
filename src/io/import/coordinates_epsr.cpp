// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "base/lineParser.h"
#include "base/sysFunc.h"
#include "io/import/coordinates.h"

// Import EPSR ATO coordinates through specified parser
bool CoordinateImportFileFormat::importEPSR(LineParser &parser, std::vector<Vec3<double>> &r)
{
    // File header:
    // Either  1   : nmols, box length, temperature   (for cubic systems)
    //    or   2   : nmols,   temperature             (for non-cubic systems)
    // followed by : A, B, C
    //             : phib, thetac, phic
    if (parser.getArgsDelim() != LineParser::Success)
        return false;
    auto nMols = parser.argi(0);
    if (parser.nArgs() == 3)
    {
        double boxSize = parser.argd(1);
        Messenger::print("File has a cubic cell (side length {} Angstroms)", boxSize);
    }
    else
    {
        Messenger::print("File has a full cell specification");
        Vec3<double> lengths, angles;
        if (parser.getArgsDelim() != LineParser::Success)
            return false;
        lengths = parser.arg3d(0);
        if (parser.getArgsDelim() != LineParser::Success)
            return false;
        angles = parser.arg3d(0);
    }

    // 2 : step sizes etc. **IGNORED**
    if (parser.getArgsDelim() != LineParser::Success)
        return false;

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
    int nAtoms, nRestraints, currentArg;
    Vec3<double> com, delta;
    r.clear();
    for (auto m = 0; m < nMols; m++)
    {
        Messenger::printVerbose("Importing molecule {} from EPSR ato file...\n", m + 1);

        if (parser.getArgsDelim() != LineParser::Success)
            return false;
        nAtoms = parser.argi(0);
        com = parser.arg3d(1);

        for (auto n = 0; n < nAtoms; n++)
        {
            // Atom name
            if (parser.getArgsDelim() != LineParser::Success)
                return false;

            // Atom coordinates (specified as offset from com)
            if (parser.getArgsDelim() != LineParser::Success)
                return false;
            delta = parser.arg3d(0);

            // Add a new atom position to our list
            r.emplace_back(com + delta);

            // Import in number of restraints line
            if (parser.getArgsDelim() != LineParser::Success)
                return false;
            nRestraints = parser.argi(0);
            currentArg = 1;
            while (nRestraints > 0)
            {
                // Look at next available argument - if none, import another line in
                if (currentArg >= parser.nArgs())
                {
                    if (parser.getArgsDelim() != LineParser::Success)
                        return false;
                    currentArg = 0;
                }
                currentArg += 2;
                --nRestraints;
            }
        }

        // Discard molecular rotations and dihedrals
        // There are 14 atoms per line - first line contains number of atoms followed by (up to) 13 indices
        if (parser.getArgsDelim() != LineParser::Success)
            return false;
        auto nRotations = parser.argi(0);
        while (nRotations > 0)
        {
            // Import line to find out which type of definition this is...
            if (parser.getArgsDelim() != LineParser::Success)
                return false;

            // Skip axis line
            if (parser.skipLines(1) != LineParser::Success)
                return false;

            // If a DIHedral, we expect an integer which defines the number of constraints, and thus the number of
            // lines to skip before the main
            if (DissolveSys::sameString(parser.argsv(0), "DIH"))
            {
                if (parser.getArgsDelim() != LineParser::Success)
                    return false;
                if (parser.skipLines(parser.argi(0)) != LineParser::Success)
                    return false;
            }

            // Finally, import in number of atoms affected by rotation and calculate next number of lines to discard
            if (parser.getArgsDelim() != LineParser::Success)
                return false;
            if (parser.skipLines(parser.argi(0) / 14) != LineParser::Success)
                return false;

            --nRotations;
        }

        atomOffset += nAtoms;
    }

    // Atomtype specifications follow, but we have the coordinates and so are done
    return true;
}

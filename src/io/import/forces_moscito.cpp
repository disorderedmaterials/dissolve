// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "base/lineParser.h"
#include "io/import/forces.h"

// Import Moscito forces through specified parser
bool ForceImportFileFormat::importMoscito(LineParser &parser, std::vector<Vec3<double>> &f)
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

    Messenger::print(" --> Importing forces in Moscito (str) format...\n");
    // Read cell lengths
    if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
        return false;

    // Read nmolecules
    if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
        return false;
    auto nMolecules = parser.argi(0);
    Messenger::print(" --> Structure file contains {} molecules.\n", nMolecules);

    // Can't pre-initialise for number of atoms as it is unknown at this point
    f.clear();

    for (auto n = 0; n < nMolecules; ++n)
    {
        // Read and discard remark and molecule label lines
        if (parser.skipLines(2) != LineParser::Success)
            return false;

        // Get number of atoms in this molecule (second integer)
        if (parser.getArgsDelim(LineParser::KeepBlanks) != LineParser::Success)
            return false;
        auto nAtoms = parser.argi(1);

        // Read in atom coordinates
        for (auto i = 0; i < nAtoms; ++i)
        {
            // Discard atom label / index line
            if (parser.skipLines(1) != LineParser::Success)
                return false;

            // Skip coordinate and velocity lines
            if (parser.skipLines(2) != LineParser::Success)
                return false;

            /*
             * Read forces (in kJ mol-1 nm-1)
             * Our internal units are 10 J mol-1 Angstrom-1:
             *
             *          kJ mol-1  =>  10 J mol-1   =   F * 100.0
             *              nm-1  =>  Angstroms-1  =   F / 10.0
             *
             * Final conversion factor =  * 10.0
             */

            // Forces are in fixed format (15.8e) with *no spacing between values*
            if (parser.readNextLine(LineParser::Defaults) != LineParser::Success)
                return false;
            std::string coords{parser.line()};
            f.emplace_back(std::stof(coords.substr(0, 15)) * 10.0, std::stof(coords.substr(15, 15)) * 10.0,
                           std::stof(coords.substr(30)) * 10.0);
        }
    }

    return true;
}

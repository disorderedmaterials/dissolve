// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "base/lineParser.h"
#include "io/import/forces.h"

// Import DL_POLY forces through specified parser
bool ForceImportFileFormat::importDLPOLY(LineParser &parser, std::vector<Vec3<double>> &f)
{
    /*
     * Read DL_POLY force information through the specified line parser.
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

    Messenger::print(" --> Reading forces in DL_POLY (CONFIG/REVCON) format...\n");
    // Skip title
    if (parser.skipLines(1) != LineParser::Success)
        return false;

    // Read in keytrj, imcon, and number of atoms, and initialise arrays
    if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
        return false;

    auto keytrj = parser.argi(0);
    auto imcon = parser.argi(1);
    auto nAtoms = parser.argi(2);
    if (keytrj != 2)
    {
        Messenger::error("File does not contain forces (keytrj = {}).\n", keytrj);
        return false;
    }
    Messenger::print(" --> Expecting forces for {} atoms (DLPOLY keytrj={}, imcon={}).\n", nAtoms, keytrj, imcon);
    f.resize(nAtoms, Vec3<double>());

    // Skip cell information if given
    if (imcon > 0)
        parser.skipLines(3);

    // Loop over atoms
    for (auto n = 0; n < nAtoms; ++n)
    {
        // Skip atomname, position and velocity lines
        if (parser.skipLines(3) != LineParser::Success)
            return false;
        if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
            return false;
        f[n] = parser.arg3d(0);
    }

    return true;
}

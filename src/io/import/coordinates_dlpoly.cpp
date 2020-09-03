/*
    *** Import - DLPOLY Coordinates
    *** src/io/import/coordinates_dlpoly.cpp
    Copyright T. Youngs 2012-2020

    This file is part of Dissolve.

    Dissolve is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Dissolve is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "base/lineparser.h"
#include "io/import/coordinates.h"

// Import DL_POLY coordinates through specified parser
bool CoordinateImportFileFormat::importDLPOLY(LineParser &parser, Array<Vec3<double>> &r)
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

    Messenger::print(" --> Importing coordinates in DL_POLY (CONFIG/REVCON) format...\n");
    // Skip title
    if (parser.skipLines(1) != LineParser::Success)
        return false;

    // Import in keytrj, imcon, and number of atoms, and initialise arrays
    if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
        return false;

    auto keytrj = parser.argi(0);
    auto imcon = parser.argi(1);
    auto nAtoms = parser.hasArg(2) ? parser.argi(2) : 0;
    Messenger::print(" --> Expecting coordinates for {} atoms (DLPOLY keytrj={}, imcon={}).\n", nAtoms, keytrj, imcon);
    r.clear();

    // Skip cell information if given
    if (imcon > 0)
        parser.skipLines(3);

    // Loop over atoms (either a specified number, or until we reach the end of the file
    auto atomCount = 0;
    while (!parser.eofOrBlank())
    {
        // Skip atomname line, get the positions, then skip velocity and force lines if necessary
        if (parser.skipLines(1) != LineParser::Success)
            return false;
        if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
            return false;
        r.add(parser.arg3d(0));
        if (parser.skipLines(keytrj) != LineParser::Success)
            return false;
        ++atomCount;
        if ((nAtoms > 0) && (atomCount == nAtoms))
            break;
    }

    return true;
}

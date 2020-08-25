/*
    *** Import - XYZ Coordinates
    *** src/io/import/coordinates_xyz.cpp
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

// Import xyz coordinates through specified parser
bool CoordinateImportFileFormat::importXYZ(LineParser &parser, Array<Vec3<double>> &r)
{
    Messenger::print(" --> Importing coordinates in xyz format...\n");

    // Read natoms
    if (parser.getArgsDelim() != LineParser::Success)
        return false;
    auto nAtoms = parser.argi(0);

    // Skip title
    if (parser.skipLines(1) != LineParser::Success)
        return false;

    Messenger::print(" --> Expecting coordinates for {} atoms.\n", nAtoms);
    r.clear();
    for (int n = 0; n < nAtoms; ++n)
    {
        if (parser.getArgsDelim() != LineParser::Success)
            return false;
        r.add(parser.arg3d(1));
    }

    return true;
}

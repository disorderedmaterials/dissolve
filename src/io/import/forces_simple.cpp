/*
    *** Import - Simple Forces
    *** src/io/import/forces_simple.cpp
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
#include "io/import/forces.h"

// Read simple formatted forces from specified file
bool ForceImportFileFormat::importSimple(LineParser &parser, Array<double> &fx, Array<double> &fy, Array<double> &fz)
{
    /*
     * Read force information through the specified line parser.
     * Assumed format is as follows:
     *
     * Line 1:    nAtoms
     * Line 2:    <atom id>    <fx>    <fy>    <fz>
     *   ...
     *
     * Commented lines are allowed, and start with '#'
     */

    // Read in number of atoms and initialise arrays
    if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
        return false;
    auto nAtoms = parser.argi(0);
    Messenger::print(" --> Expecting forces for {} atoms.\n", nAtoms);
    fx.initialise(nAtoms);
    fy.initialise(nAtoms);
    fz.initialise(nAtoms);

    for (auto n = 0; n < nAtoms; ++n)
    {
        if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
            return false;
        fx[n] = parser.argd(1);
        fy[n] = parser.argd(2);
        fz[n] = parser.argd(3);
    }

    return true;
}

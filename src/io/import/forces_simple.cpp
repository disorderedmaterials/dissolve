// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

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

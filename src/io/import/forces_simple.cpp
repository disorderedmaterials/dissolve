// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "base/lineParser.h"
#include "io/import/forces.h"

// Read simple formatted forces from specified file
bool ForceImportFileFormat::importSimple(LineParser &parser, std::vector<Vec3<double>> &f)
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
    f.resize(nAtoms, Vec3<double>());

    for (auto n = 0; n < nAtoms; ++n)
    {
        if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
            return false;
        f[n] = parser.arg3d(1);
    }

    return true;
}

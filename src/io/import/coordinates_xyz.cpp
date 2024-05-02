// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "base/lineParser.h"
#include "io/import/coordinates.h"

// Import xyz coordinates through specified parser
bool CoordinateImportFileFormat::importXYZ(LineParser &parser, std::vector<Vec3<double>> &r)
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
    for (auto n = 0; n < nAtoms; ++n)
    {
        if (parser.getArgsDelim() != LineParser::Success)
            return false;
        r.emplace_back(parser.arg3d(1));
    }

    return true;
}

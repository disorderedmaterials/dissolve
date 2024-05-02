// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "base/lineParser.h"
#include "classes/species.h"
#include "io/import/species.h"

// Import xyz coordinates through specified parser
bool SpeciesImportFileFormat::importXYZ(LineParser &parser, Species *sp)
{
    Messenger::print(" --> Importing species coordinates in xyz format...\n");

    // Read natoms
    if (parser.getArgsDelim() != LineParser::Success)
        return false;
    auto nAtoms = parser.argi(0);

    // Skip title
    if (parser.skipLines(1) != LineParser::Success)
        return false;

    Messenger::print(" --> Expecting coordinates for {} atoms.\n", nAtoms);
    sp->clear();
    for (auto n = 0; n < nAtoms; ++n)
    {
        if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
        {
            parser.closeFiles();
            Messenger::error("Couldn't read atom {} from file '{}'\n", n + 1, parser.inputFilename());
            return false;
        }
        auto Z = Elements::element(parser.argsv(0));
        sp->addAtom(Z, parser.arg3d(1), parser.hasArg(4) ? parser.argd(4) : 0.0);
    }

    return true;
}

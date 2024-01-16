// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "items/legacy.h"

// AtomTypeList (removed in 0.9.0)
bool LegacyAtomTypeListItem::deserialise(LineParser &parser, const CoreData &coreData)
{
    // First line is contains the number of items we must discard
    if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
        return false;
    return parser.skipLines(parser.argi(0)) == LineParser::Success;
}

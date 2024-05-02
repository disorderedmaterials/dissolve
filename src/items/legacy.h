// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "base/lineParser.h"

// Forward Declarations
class CoreData;

// Legacy Dummy Classes
// Enable production and deserialisation in order to support legacy file read

// AtomTypeList (removed in 0.9.0)
class LegacyAtomTypeListItem
{
    public:
    // Discard data from specified LineParser
    bool deserialise(LineParser &parser, const CoreData &coreData);
};

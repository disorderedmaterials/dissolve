// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "main/compatibility.h"
#include <functional>
#include <iostream>

namespace dissolve
{

// Upgrade from version zero, which never supported TOML in the first
// place.  This exists purely to allow testing the backward
// compatibility code before there is any pass worth checking.
SerialisedValue unrealUpgrade(SerialisedValue contents)
{
    if (contents["pairPotentials"].contains("coulombTruncation"))
        contents["pairPotentials"]["coulombTruncation"] =
            toml::find<std::string>(contents["pairPotentials"], "coulombTruncation") + "ed";

    return contents;
}

// Upgrade a file to the current version.
SerialisedValue backwardsUpgrade(SerialisedValue contents)
{
    using Version::DissolveVersion;

    // A map of version numbers and the corresponding function to
    // upgrade the file.
    std::map<std::string, std::function<SerialisedValue(SerialisedValue)>> breakingChanges = {{"1.2.0", unrealUpgrade}};

    DissolveVersion current(toml::find<std::string>(contents, "version"));

    for (auto &[key, value] : breakingChanges)
    {
        DissolveVersion next(key);
        if (current < next)
        {
            contents = value(contents);
            current = next;
        }
    }

    return contents;
}

} // namespace dissolve

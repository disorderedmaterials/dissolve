// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "main/compatibility.h"
#include <iostream>

// Upgrade from version zero, which never supported TOML in the first
// place.  This exists purely to allow testing the backward
// compatibility code before there is any pass worth checking.
SerialisedValue unreal_upgrade(SerialisedValue contents)
{
    if (contents["pairPotentials"].contains("coulombTruncation"))
        contents["pairPotentials"]["coulombTruncation"] =
            toml::find<std::string>(contents["pairPotentials"], "coulombTruncation") + "ed";

    return contents;
}

// Upgrade a file to the current version.
SerialisedValue backwards_upgrade(SerialisedValue contents)
{
    using Version::DissolveVersion;

    // A map of version numbers and the corresponding function to
    // upgrade the file.
    std::map<std::string, std::function<SerialisedValue(SerialisedValue)>> breaking_changes = {{"1.2.0", unreal_upgrade}};

    DissolveVersion current(toml::find<std::string>(contents, "version"));

    for (auto &[key, value] : breaking_changes)
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

// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "main/compatibility.h"

SerialisedValue unreal_upgrade(SerialisedValue contents)
{
    contents["unreal"] = false;
    return contents;
}

SerialisedValue backwards_upgrade(SerialisedValue contents)
{
    using Version::DissolveVersion;
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

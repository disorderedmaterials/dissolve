// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#include "main/version.h"
#include <fmt/core.h>

#define DISSOLVEVERSION "0.7.0"
#define DISSOLVESHORTHASH ""
#define DISSOLVEREPO "https://github.com/projectdissolve/dissolve.git"

namespace Version
{

std::string_view info()
{
    static std::string versionString;

    if (versionString.empty())
    {
        if (std::string_view(DISSOLVESHORTHASH).empty())
            versionString = DISSOLVEVERSION;
        else
            versionString = fmt::format("{} @ {}", DISSOLVEVERSION, DISSOLVESHORTHASH);
    }

    return versionString;
}

// Return repo url
std::string_view repoUrl() { return DISSOLVEREPO; }

}; // namespace Version

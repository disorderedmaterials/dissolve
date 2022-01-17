// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "main/version.h"
#define FMT_HEADER_ONLY
#include <fmt/format.h>

#define DISSOLVEVERSION "0.9.0"
#define DISSOLVESHORTHASH ""
#define DISSOLVEREPO "https://github.com/disorderedmaterials/dissolve.git"

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

// Return app type
std::string_view appType()
{
#ifdef MULTITHREADING
#ifdef PARALLEL
    return "MPI/Threads";
#else
    return "Multithreaded";
#endif
#else
    return "Serial";
#endif
}

}; // namespace Version

// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "main/version.h"
#include <fmt/format.h>
#include <iostream>

#define DISSOLVEVERSION "1.5.1"
#define DISSOLVESHORTHASH ""
#define DISSOLVEREPO "https://github.com/disorderedmaterials/dissolve.git"

namespace Version
{
// Return semantic version number
std::string_view semantic()
{
    static std::string versionString = fmt::format("{}", DISSOLVEVERSION);
    return versionString;
}

// Return current version information, including short hash if defined
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

DissolveVersion::DissolveVersion(std::string_view version)
{
    auto dot = version.find(".");
    major_ = std::stoi(std::string(version.substr(0, dot)));
    auto rest = version.substr(dot + 1);
    dot = rest.find(".");
    minor_ = std::stoi(std::string(rest.substr(0, dot)));
    patch_ = std::stoi(std::string(rest.substr(dot + 1)));
}

bool DissolveVersion::operator<(const DissolveVersion &other) const
{
    if (major_ != other.major_)
        return major_ < other.major_;
    if (minor_ != other.minor_)
        return minor_ < other.minor_;
    return patch_ < other.patch_;
}

bool DissolveVersion::operator==(const DissolveVersion &other) const
{
    return major_ == other.major_ && minor_ == other.minor_ && patch_ == other.patch_;
}

std::ostream &operator<<(std::ostream &os, const DissolveVersion &version)
{
    os << fmt::format("{}.{}.{}", version.major_, version.minor_, version.patch_);
    return os;
}
}; // namespace Version

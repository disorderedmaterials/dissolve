// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include <string_view>

namespace Version
{
// Return semantic version number
std::string_view semantic();
// Return current version information, including short hash if defined
std::string_view info();
// Return repo url
std::string_view repoUrl();
// Return app type
std::string_view appType();

// This class handles the version numbers of the app in a structured
// manner.
class DissolveVersion
{
    private:
    int major_;
    int minor_;
    int patch_;

    public:
    DissolveVersion(std::string_view version);
    bool operator<(const DissolveVersion &other) const;
    bool operator==(const DissolveVersion &other) const;
    friend std::ostream &operator<<(std::ostream &os, const DissolveVersion &version);
};

}; // namespace Version

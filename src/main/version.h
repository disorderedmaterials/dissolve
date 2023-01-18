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
}; // namespace Version

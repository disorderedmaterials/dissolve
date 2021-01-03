// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include <string_view>

namespace Version
{

// Return current version information
std::string_view info();
// Return repo url
std::string_view repoUrl();

}; // namespace Version

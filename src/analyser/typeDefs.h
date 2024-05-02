// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include <map>
#include <vector>

// Forward Declarations
class Site;

namespace Analyser
{
// Basic Site Data - Pointer to site and index of site in its stack
using SiteData = std::tuple<const Site *, int>;
// Vector of Site Data
using SiteVector = std::vector<SiteData>;
// Map of Site to Sites
using SiteMap = std::map<const Site *, SiteVector>;
}; // namespace Analyser

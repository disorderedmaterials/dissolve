// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "data/elements.h"
#include <array>

// Element Colours
namespace ElementColours
{

// Return colour for specified element
const std::array<float, 4> &colour(Elements::Element Z);

} // namespace ElementColours

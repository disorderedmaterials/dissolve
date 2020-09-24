// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#pragma once

#include "data/elements.h"

// Element Colours
namespace ElementColours
{

// Return colour for specified Z
const float *colour(int Z);

// Return colour for specified Element
const float *colour(Element *el);

} // namespace ElementColours

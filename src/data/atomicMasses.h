// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "data/elements.h"

namespace AtomicMass
{

// Return atomic mass for specified Z
double mass(Elements::Element Z);
// Return reduced mass for specified pair of Z's
double reducedMass(Elements::Element Z1, Elements::Element Z2);

}; // namespace AtomicMass

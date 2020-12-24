// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "data/elements.h"

namespace AtomicMass
{

// Return atomic mass for specified Z
double mass(int Z);
// Return reduced mass for specified pair of Z's
double reducedMass(int Z1, int Z2);
// Return reduced mass for specified pair of Elements
double reducedMass(Elements::Element *e1, Elements::Element *e2);

}; // namespace AtomicMass

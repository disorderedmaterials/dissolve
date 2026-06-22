// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "base/applicative.h"
#include "math/vector3.h"

namespace parsers
{

// A parser that accepts an integer greater than or equal to zero
Parser<int> natural();

// A parser that accepts an integer
Parser<int> integer();

// A parser that accepts a real, floating point number
Parser<double> real();

Parser<Vector3> vector3();
}; // namespace parsers

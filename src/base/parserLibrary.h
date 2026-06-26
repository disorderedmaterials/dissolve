// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "base/applicative.h"
#include "math/matrix3.h"
#include "math/vector3.h"
#include "math/vector3i.h"

namespace Parsers
{

// A parser that accepts an integer greater than or equal to zero
Parser<int> natural();

// A parser that accepts an integer
Parser<int> integer();

// A parser that accepts a real, floating point number
Parser<double> real();

// A parser that accepts a 3-vector of floating point numbers
Parser<Vector3> vector3();
// A parser that accepts a 3-vector of integers
Parser<Vector3i> vector3i();

// A parser that accepts a 3x3 matrix of floating point numbers
Parser<Matrix3> matrix3();

}; // namespace Parsers

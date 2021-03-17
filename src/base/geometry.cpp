// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "base/geometry.h"

// Forward Declarations
/* none */

Geometry::Geometry()
{
    indices_[0] = -1;
    indices_[1] = -1;
    indices_[2] = -1;
    indices_[3] = -1;
    value_ = 0.0;
}

Geometry::~Geometry() {}

/*
 * Data
 */

// Set reference value and indices
void Geometry::set(double value, int a, int b, int c, int d)
{
    indices_[0] = a;
    indices_[1] = b;
    indices_[2] = c;
    indices_[3] = d;
    value_ = value;
}

// Return reference value
double Geometry::value() { return value_; }

// Return index value
int Geometry::indices(int i) const { return indices_[i]; }

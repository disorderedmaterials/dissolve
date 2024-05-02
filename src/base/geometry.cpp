// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "base/geometry.h"

Geometry::Geometry()
{
    indices_[0] = -1;
    indices_[1] = -1;
    indices_[2] = -1;
    indices_[3] = -1;
    value_ = 0.0;
}

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
double Geometry::value() const { return value_; }

// Return index value
int Geometry::indices(int i) const { return indices_[i]; }

bool Geometry::operator==(const Geometry &rhs) const { return value_ == rhs.value_ && indices_ == rhs.indices_; }
bool Geometry::operator!=(const Geometry &rhs) const { return !(rhs == *this); }

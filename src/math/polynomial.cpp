// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "math/polynomial.h"
#include <cassert>
#include <cmath>

Quadratic::Quadratic(double a, double b, double c) : a_(a), b_(b), c_(c) {}

double Quadratic::at(double x) const { return a_ * x * x + b_ * x + c_; }

bool Quadratic::hasRoots() const { return b_ * b_ > 4 * a_ * c_; }

std::pair<double, double> Quadratic::roots() const
{
    assert(hasRoots());
    auto det = std::sqrt(b_ * b_ - 4 * a_ * c_);
    return {(-b_ - det) / (2 * a_), (-b_ + det) / (2 * a_)};
}

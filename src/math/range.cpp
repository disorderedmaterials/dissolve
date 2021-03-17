// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "math/range.h"

Range::Range(double minimum, double maximum)
{
    minimum_ = minimum;
    maximum_ = maximum;
}

/*
 * Data
 */

// Set range
void Range::set(double minimum, double maximum)
{
    minimum_ = minimum;
    maximum_ = maximum;
}

// Set minimum for range
void Range::setMinimum(double minimum) { minimum_ = minimum; }

// Return minimum for range
double Range::minimum() const { return minimum_; }

// Set maximum for range
void Range::setMaximum(double maximum) { maximum_ = maximum; }

// Return maximum for range
double Range::maximum() const { return maximum_; }

// Return whether the range contains the specified value
bool Range::contains(double d) const
{
    if ((d < minimum_) || (d > maximum_))
        return false;
    return true;
}

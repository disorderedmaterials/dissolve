// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

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

// Express as a serialisable value
SerialisedValue Range::serialise() const { return {{"min", minimum_}, {"max", maximum_}}; }

// Read values from a serialisable value
void Range::deserialise(const SerialisedValue &node)
{
    minimum_ = toml::find<double>(node, "min");
    maximum_ = toml::find<double>(node, "max");
}

// Equality
bool Range::operator==(const Range &rhs) const { return minimum_ == rhs.minimum_ && maximum_ == rhs.maximum_; }

bool Range::operator!=(const Range &rhs) const { return !(rhs == *this); }

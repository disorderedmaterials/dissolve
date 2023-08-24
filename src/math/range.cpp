// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "math/range.h"

Range::Range(std::optional<double> minimum, std::optional<double> maximum) { set(minimum, maximum); }

/*
 * Data
 */

// Set range
void Range::set(std::optional<double> minimum, std::optional<double> maximum)
{
    minimum_ = minimum;
    maximum_ = maximum;
    if (isDefined() && minimum_ > maximum_)
    {
        std::swap(minimum_, maximum_);
    }
}

// Set range
void Range::set(double minimum, double maximum)
{
    minimum_ = minimum;
    maximum_ = maximum;
    if (minimum_ > maximum_)
    {
        std::swap(minimum_, maximum_);
    }
}

// Set minimum for range
void Range::setMinimum(double minimum) { set(minimum, maximum_); }

// Return minimum for range
double Range::minimum() const { return minimum_ ? *minimum_ : 0.0; }

// Set maximum for range
void Range::setMaximum(double maximum) { set(minimum_, maximum); }

// Return maximum for range
double Range::maximum() const { return maximum_ ? *maximum_ : 0.0; }

// Return whether the range contains the specified value
bool Range::contains(double d) const { return isDefined() && (d >= minimum_) && (d <= maximum_); }

bool Range::isDefined() const { return (minimum_ && maximum_); }

// Express as a serialisable value
SerialisedValue Range::serialise() const { return {{"min", minimum()}, {"max", maximum()}}; }

// Read values from a serialisable value
void Range::deserialise(const SerialisedValue &node)
{
    minimum_ = toml::find<double>(node, "min");
    maximum_ = toml::find<double>(node, "max");
}

// Equality
bool Range::operator==(const Range &rhs) const { return minimum() == rhs.minimum_ && maximum() == rhs.maximum_; }

bool Range::operator!=(const Range &rhs) const { return !(rhs == *this); }

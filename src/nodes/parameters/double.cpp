// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "nodes/parameters/double.h"

DoubleParameter::DoubleParameter(double &data, std::optional<double> minValue, std::optional<double> maxValue)
    : data_(data), default_(data), minimumLimit_(minValue), maximumLimit_(maxValue), type_(ParameterDataType::Number)
{
}

/*
 * Data
 */

// Set data
bool DoubleParameter::set(double value)
{
    data_ = value;

    // Clamp value to limits
    if (minimumLimit_ && value < minimumLimit_.value())
        data_ = *minimumLimit_;
    else if (maximumLimit_ && value > maximumLimit_)
        data_ = *maximumLimit_;

    return true;
}

// Return data
double DoubleParameter::get() const { return data_; }

// Return minimum limit
std::optional<int> DoubleParameter::minimumLimit() const { return minimumLimit_; }

// Return maximum limit
std::optional<int> DoubleParameter::maximumLimit() const { return maximumLimit_; }

/*
 * I/O
 */

// Express as a serialised value
SerialisedValue DoubleParameter::serialise() const { return data_; }

// Read from a serialised value
void DoubleParameter::deserialise(const SerialisedValue &node) { set(node.as_floating()); }

// Has not changed from initial value
bool DoubleParameter::isDefault() const { return data_ == default_; }

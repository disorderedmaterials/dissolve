// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "nodes/parameters/integer.h"

IntegerParameter::IntegerParameter(int &data, std::optional<int> minValue, std::optional<int> maxValue)
    : data_(data), default_(data), minimumLimit_(minValue), maximumLimit_(maxValue), type_(ParameterDataType::Number)
{
}

/*
 * Data
 */

// Set data
bool IntegerParameter::set(int value)
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
int IntegerParameter::get() const { return data_; }

// Return minimum limit
std::optional<int> IntegerParameter::minimumLimit() const { return minimumLimit_; }

// Return maximum limit
std::optional<int> IntegerParameter::maximumLimit() const { return maximumLimit_; }

/*
 * I/O
 */

// Express as a serialisable value
SerialisedValue IntegerParameter::serialise() const { return data_; }

// Read values from a serialisable value
void IntegerParameter::deserialise(const SerialisedValue &node) { set(node.as_integer()); }

// Has not changed from initial value
bool IntegerParameter::isDefault() const { return data_ == default_; }

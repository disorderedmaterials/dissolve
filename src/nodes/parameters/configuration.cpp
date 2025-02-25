// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "nodes/parameters/configuration.h"

ConfigurationParameter::ConfigurationParameter(Configuration *&data)
    : data_(data), default_(data), type_(ParameterDataType::ConfigurationPointer)
{
}

/*
 * Data
 */

// Set data
bool ConfigurationParameter::set(Configuration *value)
{
    data_ = value;

    return true;
}

// Return data
Configuration *ConfigurationParameter::get() const { return data_; }

/*
 * I/O
 */

// Express as a serialisable value
SerialisedValue ConfigurationParameter::serialise() const { return data_; }

// Read values from a serialisable value
void ConfigurationParameter::deserialise(const SerialisedValue &node) { // Never required; }

// Has not changed from initial value
bool ConfigurationParameter::isDefault() const { return data_ == default_; }

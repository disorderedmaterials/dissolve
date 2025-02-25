// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "nodes/parameter.h"

// Forward Declarations
class Configuration;

// Configuration Parameter
class ConfigurationParameter : public Parameter
{
    public:
    explicit ConfigurationParameter(Configuration *&data);

    /*
     * Data
     */
    private:
    // Reference to target data
    Configuration *&data_;
    // Initial value
    const Configuration *default_{nullptr};
    // Optional limits to apply
    std::optional<int> minimumLimit_, maximumLimit_;

    public:
    // Set data
    bool set(Configuration *value);
    // Return data
    Configuration *get() const;
    // Return whether the contained data represents the default value
    bool isDefault() const override;

    /*
     * I/O
     */
    public:
    // Express as a serialised value
    SerialisedValue serialise() const override;
    // Read from a serialised value
    void deserialise(const SerialisedValue &node) override;
};

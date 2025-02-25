// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "nodes/parameters/parameter.h"
#include <optional>

// Integer Parameter
class IntegerParameter : public Parameter
{
    public:
    explicit IntegerParameter(int &data, std::optional<int> minValue = std::nullopt, std::optional<int> maxValue = std::nullopt);

    /*
     * Data
     */
    private:
    // Reference to target data
    int &data_;
    // Initial value
    const int default_;
    // Optional limits to apply
    std::optional<int> minimumLimit_, maximumLimit_;

    public:
    // Set data
    bool set(int value);
    // Return data
    int get() const;
    // Return whether the contained data represents the default value
    bool isDefault() const override;
    // Return minimum limit
    std::optional<int> minimumLimit() const;
    // Return maximum limit
    std::optional<int> maximumLimit() const;

    /*
     * I/O
     */
    public:
    // Express as a serialised value
    SerialisedValue serialise() const override;
    // Read from a serialised value
    void deserialise(const SerialisedValue &node) override;

};

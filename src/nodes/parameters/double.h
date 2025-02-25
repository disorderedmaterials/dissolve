// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "nodes/parameters/parameter.h"
#include <optional>

// Double Parameter
class DoubleParameter : public Parameter
{
    public:
    explicit DoubleParameter(double &data, std::optional<double> minValue = std::nullopt, std::optional<double> maxValue = std::nullopt);

    /*
     * Data
     */
    private:
    // Reference to target data
    double &data_;
    // Initial value
    const double default_;
    // Optional limits to apply
    std::optional<double> minimumLimit_, maximumLimit_;

    public:
    // Set data
    bool set(double value);
    // Return data
    double get() const;
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
    // Express as a serialisable value
    SerialisedValue serialise() const override;
    // Read from a serialisable value
    void deserialise(const SerialisedValue &node) override;

};

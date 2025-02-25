// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "nodes/parameter.h"
#include <optional>

// Parameter managing optional double data with a default text value
class OptionalDoubleParameter : public Parameter
{
    public:
    explicit OptionalDoubleParameter(std::optional<double> &data, double minValue, std::optional<double> maxValue,
                                   double valueDelta, std::string_view textWhenNull);
    ~OptionalDoubleParameter() override = default;

    /*
     * Data
     */
    private:
    // Reference to target data
    std::optional<double> &data_;
    // Minimum limit, at or below which equates to nullopt
    double minimumLimit_;
    // Optional maximum limit to apply
    std::optional<double> maximumLimit_;
    // Step size for widget
    double valueDelta_;
    // Null value text
    std::string textWhenNull_;

    public:
    // Set data
    bool set(std::optional<double> value);
    // Return data
    std::optional<double> get() const;
    // Return whether the contained data represents the default value
    bool isDefault() const override;
    // Return minimum limit
    double minimumLimit() const;
    // Return maximum limit
    std::optional<double> maximumLimit() const;
    // Return step size for widget
    double valueDelta() const;
    // Return null value text
    std::string textWhenNull() const;

    /*
     * Arguments
     */
    public:
    // Express as a serialised value
    SerialisedValue serialise() const override;
    // Read from a serialised value
    void deserialise(const SerialisedValue &node) override;
};

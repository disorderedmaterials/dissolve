// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "base/serialiser.h"
#include "math/matrix4.h"
#include <optional>

// Range
class Range : public Serialisable<>
{
    public:
    Range(std::optional<double> minimum = std::nullopt, std::optional<double> maximum = std::nullopt);

    /*
     * Data
     */
    private:
    // Minimum and maximum for range
    std::optional<double> minimum_, maximum_;

    private:
    // Set range
    void set(std::optional<double> minimum, std::optional<double> maximum);

    public:
    // Set range
    void set(double minimum, double maximum);
    // Set minimum for range
    void setMinimum(double minimum);
    // Return minimum for range
    double minimum() const;
    // Set maximum for range
    void setMaximum(double maximum);
    // Return maximum for range
    double maximum() const;
    // Return whether the range contains the specified value
    bool contains(double d) const;
    // Return whether or not the range has been fully defined
    bool isDefined() const;
    // Express as a serialisable value
    SerialisedValue serialise() const override;
    // Read values from a serialisable value
    void deserialise(const SerialisedValue &node) override;
    bool operator==(const Range &rhs) const;
    bool operator!=(const Range &rhs) const;
};

// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#pragma once

#include "base/serialiser.h"
#include "math/matrix4.h"

// Range
class Range : public Serialisable<>
{
    public:
    Range(double minimum = 0.0, double maximum = 0.0);

    /*
     * Data
     */
    private:
    // Minimum and maximum for range
    double minimum_, maximum_;

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
    // Express as a tree node
    SerialisedValue serialise() const override;
    // Read values from a tree node
    void deserialise(const SerialisedValue &node) override;
};

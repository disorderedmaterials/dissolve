// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

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
    // Express as a serialisable value
    SerialisedValue serialise() const override;
    // Read values from a serialisable value
    void deserialise(const SerialisedValue &node) override;
    int rangeToIndex(double firstElem, double secondElem, double boundary);
    bool operator==(const Range &rhs) const;
    bool operator!=(const Range &rhs) const;
};

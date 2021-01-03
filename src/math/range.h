// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "math/matrix4.h"

// Range
class Range
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
};

// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#pragma once

// Forward Declarations
class Data1D;

// Regression
class Regression
{
    public:
    // Return gradient of last n points
    static double linear(const Data1D &data, int nSamples);
    // Return gradient of last n points, along with average y value
    static double linear(const Data1D &data, int nSamples, double &yBar);
};

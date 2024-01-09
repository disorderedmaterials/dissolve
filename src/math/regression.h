// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

// Forward Declarations
class Data1D;

// Regression
namespace Regression
{
// Return gradient of last n points
double linear(const Data1D &data, int nSamples);
// Return gradient of last n points, along with average y value
double linear(const Data1D &data, int nSamples, double &yBar);
}; // namespace Regression

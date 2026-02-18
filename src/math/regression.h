// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "templates/optionalRef.h"
#include <vector>

// Forward Declarations
class Data1D;

// Regression
namespace Regression
{
// Return gradient of last n points, along with average y value
double linearGradient(const Data1D &data, int nSamples, OptionalReferenceWrapper<double> returnYBar = {});
// Return linear gradient of last n points, along with average y value, assuming unit spacing between points
double linearGradient(const std::vector<double> &y, int nSamples, OptionalReferenceWrapper<double> returnYBar = {});
}; // namespace Regression

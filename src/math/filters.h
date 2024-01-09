// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "math/function1D.h"

// Forward Declarations
class Data1D;

// Filters
namespace Filters
{
// Perform point-wise convolution of data with the supplied BroadeningFunction
void convolve(Data1D &data, const Functions::Function1DWrapper function, bool variableOmega = false, bool normalise = false);
// Perform convolution of the supplied delta function into the supplied data
void convolve(double xCentre, double value, const Functions::Function1DWrapper function, Data1D &dest);
// Apply Kolmogorov–Zurbenko filter to data
void kolmogorovZurbenko(Data1D &data, int k, int m, bool normalised = false);
// Apply median filter to data
void median(Data1D &data, int length);
// Perform moving average smoothing
void movingAverage(std::vector<double> &data, int length);
void movingAverage(Data1D &data, int length);
// Perform moving average smoothing on data, normalising area after smooth
void normalisedMovingAverage(Data1D &data, int length);
// Subtract average level (starting at supplied x value) from data
double subtractAverage(Data1D &data, double xStart);
// Trim supplied data to specified range
void trim(Data1D &data, double xMin, double xMax, bool interpolateEnds = false, double interpolationThreshold = 0.01);
// Trim supplied data to be the same range as the reference data
void trim(Data1D &data, const Data1D &ref, bool interpolateEnds = false, double interpolationThreshold = 0.01);
// Convert bin boundaries to centre-bin values
void convertBinBoundaries(Data1D &data);
}; // namespace Filters

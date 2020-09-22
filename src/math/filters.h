// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#pragma once

#include "math/broadeningfunction.h"

// Forward Declarations
class Data1D;

// Filters
class Filters
{
    public:
    // Perform point-wise convolution of data with the supplied BroadeningFunction
    static void convolve(Data1D &data, const BroadeningFunction &function, bool variableOmega = false, bool normalise = true);
    // Perform convolution of the supplied delta function into the supplied data
    static void convolve(double xCentre, double value, const BroadeningFunction &function, Data1D &dest);
    // Apply Kolmogorov–Zurbenko filter to data
    static void kolmogorovZurbenko(Data1D &data, int k, int m, bool normalised = false);
    // Apply median filter to data
    static void median(Data1D &data, int length);
    // Perform moving average smoothing on data
    static void movingAverage(Data1D &data, int avgSize);
    // Perform moving average smoothing on data, normalising area after smooth
    static void normalisedMovingAverage(Data1D &data, int avgSize);
    // Subtract average level (starting at supplied x value) from data
    static double subtractAverage(Data1D &data, double xStart);
    // Trim supplied data to specified range
    static void trim(Data1D &data, double xMin, double xMax, bool interpolateEnds = false,
                     double interpolationThreshold = 0.01);
    // Convert bin boundaries to centre-bin values
    static void convertBinBoundaries(Data1D &data);
};

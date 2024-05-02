// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "math/data2D.h"
#include "math/sampledDouble.h"
#include "templates/array2D.h"

// Two-Dimensional Histogram
class Histogram2D
{
    public:
    Histogram2D();
    ~Histogram2D() = default;
    Histogram2D(const Histogram2D &source);
    // Clear data
    void clear();

    /*
     * Histogram Data
     */
    private:
    // Minimum value for x data (hard left-edge of first bin)
    double xMinimum_;
    // Maximum value for x data (hard right-edge of last bin, adjusted to match bin width if necessary)
    double xMaximum_;
    // Bin width for x axis
    double xBinWidth_;
    // Number of bins along x
    int nXBins_;
    // Minimum value for y data (hard left-edge of first bin)
    double yMinimum_;
    // Maximum value for y data (hard right-edge of last bin, adjusted to match bin width if necessary)
    double yMaximum_;
    // Bin width for y axis
    double yBinWidth_;
    // Number of bins along y
    int nYBins_;
    // Histogram bins
    Array2D<long int> bins_;
    // Array of x bin centres
    std::vector<double> xBinCentres_;
    // Array of y bin centres
    std::vector<double> yBinCentres_;
    // Accumulated averages
    Array2D<SampledDouble> averages_;
    // Number of values binned over all bins
    long int nBinned_;
    // Number of points missed (out of bin range)
    long int nMissed_;
    // Accumulated data
    Data2D accumulatedData_;

    public:
    // Initialise with specified bin range
    void initialise(double xMinimum, double xMaximum, double xBinWidth, double yMinimum, double yMaximum, double yBinWidth);
    // Zero histogram bins
    void zeroBins();
    // Return minimum value for x data (hard left-edge of first bin)
    double xMinimum() const;
    // Return maximum value for x data (hard right-edge of last bin, adjusted to match bin width if necessary)
    double xMaximum() const;
    // Return x bin width
    double xBinWidth() const;
    // Return number of x bins
    int nXBins() const;
    // Return minimum value for y data (hard left-edge of first bin)
    double yMinimum() const;
    // Return maximum value for y data (hard right-edge of last bin, adjusted to match bin width if necessary)
    double yMaximum() const;
    // Return y bin width
    double yBinWidth() const;
    // Return number of y bins
    int nYBins() const;
    // Bin specified value, returning success
    bool bin(double x, double y);
    // Return number of values binned over all bins
    long int nBinned() const;
    // Accumulate current histogram bins into averages
    void accumulate();
    // Return Array of x centre-bin values
    const std::vector<double> &xBinCentres() const;
    // Return Array of y centre-bin values
    const std::vector<double> &yBinCentres() const;
    // Return histogram data
    Array2D<long int> &bins();
    // Add source histogram data into local array
    void add(Histogram2D &other, int factor = 1);
    // Return accumulated (averaged) data
    const Data2D &accumulatedData() const;

    /*
     * Operators
     */
    public:
    void operator=(const Histogram2D &source);

    /*
     * Serialisation
     */
    public:
    // Read data through specified LineParser
    bool deserialise(LineParser &parser);
    // Write data through specified LineParser
    bool serialise(LineParser &parser) const;

    /*
     * Parallel Comms
     */
    public:
    // Sum histogram data onto all processes
    bool allSum(ProcessPool &procPool);
};

// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "math/data1D.h"
#include "math/sampledDouble.h"

// One-Dimensional Histogram
class Histogram1D
{
    public:
    Histogram1D();
    ~Histogram1D() = default;
    Histogram1D(const Histogram1D &source);
    // Clear data
    void clear();

    /*
     * Histogram Data
     */
    private:
    // Minimum value for data (hard left-edge of first bin)
    double minimum_;
    // Maximum value for data (hard right-edge of last bin, adjusted to match bin width if necessary)
    double maximum_;
    // Bin width
    double binWidth_;
    // Number of bins
    int nBins_;
    // Histogram bins
    std::vector<long int> bins_;
    // Array of bin centres
    std::vector<double> binCentres_;
    // Accumulated averages
    std::vector<SampledDouble> averages_;
    // Number of values binned over all bins
    long int nBinned_;
    // Number of points missed (out of bin range)
    long int nMissed_;
    // Accumulated data
    Data1D accumulatedData_;

    private:
    // Update accumulated data
    void updateAccumulatedData();

    public:
    // Initialise with specified bin range
    void initialise(double minimum, double maximum, double binWidth);
    // Zero histogram bins
    void zeroBins();
    // Set up supplied axis
    static void setUpAxis(double axisMin, double &axisMax, double binWidth, int &nBins, std::vector<double> &binCentres);
    // Return minimum value for data (hard left-edge of first bin)
    double minimum() const;
    // Return maximum value for data (hard right-edge of last bin, adjusted to match bin width if necessary)
    double maximum() const;
    // Return bin width
    double binWidth() const;
    // Return number of bins
    int nBins() const;
    // Bin specified value, returning success
    bool bin(double x);
    // Return number of values binned over all bins
    long int nBinned() const;
    // Accumulate current histogram bins into averages
    void accumulate();
    // Return Array of x centre-bin values
    const std::vector<double> &binCentres() const;
    // Return histogram data
    std::vector<long int> &bins();
    const std::vector<long int> &bins() const;
    // Add source histogram data into local array
    void add(Histogram1D &other, int factor = 1);
    // Return current data
    Data1D data() const;
    // Return accumulated (averaged) data
    const Data1D &accumulatedData() const;

    /*
     * Operators
     */
    public:
    void operator=(const Histogram1D &source);
    Histogram1D operator+(const Histogram1D &other) const;

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
    bool allSum(const ProcessPool &procPool, OptionalReferenceWrapper<Timer> commsTimer = {});
};

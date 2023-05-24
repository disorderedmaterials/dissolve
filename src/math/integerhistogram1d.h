// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "math/data1d.h"
#include "math/sampleddouble.h"

// Pure Integer Histogram
class IntegerHistogram1D
{
    public:
    IntegerHistogram1D();
    ~IntegerHistogram1D() = default;
    IntegerHistogram1D(const IntegerHistogram1D &source);
    // Clear data
    void clear();

    /*
     * Histogram Data
     */
    private:
    // Minimum value for data (hard left-edge of first bin)
    int minimum_;
    // Maximum value for data (hard right-edge of last bin, adjusted to match bin width if necessary)
    int maximum_;
    // Bin width
    int binWidth_;
    // Number of bins
    int nBins_;
    // Histogram bins
    std::vector<long int> bins_;
    // Array of bin centres
    std::vector<int> binCentres_;
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
    void initialise(int minimum, int maximum, int binWidth);
    // Zero histogram bins
    void zeroBins();
    // Set up supplied axis
    static void setUpAxis(int axisMin, int &axisMax, int binWidth, int &nBins, std::vector<int> &binCentres);
    // Return minimum value for data (hard left-edge of first bin)
    int minimum() const;
    // Return maximum value for data (hard right-edge of last bin, adjusted to match bin width if necessary)
    int maximum() const;
    // Return bin width
    int binWidth() const;
    // Return number of bins
    int nBins() const;
    // Bin specified value, returning success
    bool bin(int x);
    // Return number of values binned over all bins
    long int nBinned() const;
    // Accumulate current histogram bins into averages
    void accumulate();
    // Return Array of x centre-bin values
    const std::vector<int> &binCentres() const;
    // Return histogram data
    std::vector<long int> &bins();
    const std::vector<long int> &bins() const;
    // Add source histogram data into local array
    void add(IntegerHistogram1D &other, int factor = 1);
    // Return current data
    Data1D data() const;
    // Return accumulated (averaged) data
    const Data1D &accumulatedData() const;

    /*
     * Operators
     */
    public:
    void operator=(const IntegerHistogram1D &source);
    IntegerHistogram1D operator+(const IntegerHistogram1D &other) const;

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
